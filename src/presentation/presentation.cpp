#include "presentation.h"

#include <iostream>
#include <limits>
#include <string>

Presentation::Presentation() = default;
Presentation::~Presentation() = default;

bool Presentation::ensureReady() {
    m_lastError = QSqlError();
    if (!m_model.ensureInitialized()) {
        m_lastError = m_model.lastError();
        return false;
    }
    if (!m_repo) {
        m_repo = std::make_unique<PasswordRepository>(m_model.database());
    }
    return true;
}

void Presentation::printHeader() const {
    std::cout << "\n===== Gerenciador de Senhas =====\n";
}

void Presentation::printMenu() const {
    std::cout << "(1) Gerar senha (14 chars) e salvar\n";
    std::cout << "(2) Salvar/Atualizar senha manualmente\n";
    std::cout << "(3) Buscar senha por nome\n";
    std::cout << "(4) Sair\n";
    std::cout << "Escolha: ";
}

void Presentation::clearStdin() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string Presentation::readLine(const char* prompt) {
    std::cout << prompt;
    std::string s;
    std::getline(std::cin, s);
    return s;
}

void Presentation::run() {
    if (!ensureReady()) {
        std::cerr << "Erro ao inicializar o banco: "
                  << m_lastError.text().toStdString() << "\n";
        return;
    }

    while (true) {
        printHeader();
        printMenu();

        int op = 0;
        if (!(std::cin >> op)) {
            std::cout << "Entrada inválida. Tente novamente.\n";
            clearStdin();
            continue;
        }
        clearStdin(); // consome '\n'

        switch (op) {
            case 1: handleGenerate(); break;
            case 2: handleSave();     break;
            case 3: handleFetch();    break;
            case 4:
                std::cout << "Saindo...\n";
                return;
            default:
                std::cout << "Opção inválida.\n";
                break;
        }
    }
}

// ===== Opção 1: gerar + (opcional) salvar =====
void Presentation::handleGenerate() {
    const auto generated = generator_password();
    const QString qpass = QString::fromStdString(generated);

    if (qpass.isEmpty()) {
        std::cerr << "Falha ao gerar senha.\n";
        return;
    }

    std::cout << "Senha gerada (14): " << qpass.toStdString() << "\n";

    const std::string saveOpt = readLine("Deseja salvar? (s/N): ");
    if (!saveOpt.empty() && (saveOpt[0] == 's' || saveOpt[0] == 'S')) {
        const std::string name = readLine("Nome (chave): ");
        if (name.empty()) {
            std::cout << "Nome vazio. Operação cancelada.\n";
            return;
        }
        if (!ensureReady()) {
            std::cerr << "Erro: " << m_lastError.text().toStdString() << "\n";
            return;
        }
        PasswordRow row{ QString::fromStdString(name), qpass };
        if (!m_repo->upsert(row)) {
            std::cerr << "Falha ao salvar: "
                      << m_repo->lastError().text().toStdString() << "\n";
        } else {
            std::cout << "Salvo com sucesso.\n";
        }
    }
}

// ===== Opção 2: salvar/atualizar manual =====
void Presentation::handleSave() {
    const std::string name   = readLine("Nome (chave): ");
    const std::string secret = readLine("Senha (valor): ");

    if (name.empty() || secret.empty()) {
        std::cout << "Nome e senha são obrigatórios.\n";
        return;
    }
    if (!ensureReady()) {
        std::cerr << "Erro: " << m_lastError.text().toStdString() << "\n";
        return;
    }
    PasswordRow row{ QString::fromStdString(name),
                     QString::fromStdString(secret) };
    if (!m_repo->upsert(row)) {
        std::cerr << "Falha ao salvar/atualizar: "
                  << m_repo->lastError().text().toStdString() << "\n";
    } else {
        std::cout << "OK.\n";
    }
}

// ===== Opção 3: buscar por nome =====
void Presentation::handleFetch() {
    const std::string name = readLine("Nome para buscar: ");
    if (name.empty()) {
        std::cout << "Nome é obrigatório.\n";
        return;
    }
    if (!ensureReady()) {
        std::cerr << "Erro: " << m_lastError.text().toStdString() << "\n";
        return;
    }
    auto res = m_repo->getByName(QString::fromStdString(name));
    if (!res.has_value()) {
        if (m_repo->lastError().isValid()) {
            std::cerr << "Erro na busca: "
                      << m_repo->lastError().text().toStdString() << "\n";
        } else {
            std::cout << "Não encontrada.\n";
        }
    } else {
        std::cout << "Senha: " << res->secret.toStdString() << "\n";
    }
}
