#pragma once

#include <QString>
#include <QSqlError>
#include <memory>

// Domínio (modelo + repositório)
#include "password_db/models/model.h"
#include "password_db/repositories/repository.h"

// Gerador de senhas (sua função): retorna uma senha de 14 caracteres
#include "generate_password/generator_password.h"

// Camada de apresentação (console/UI de texto)
// - exibe menu
// - lê as opções do usuário
// - delega ao Repository as operações de CRUD
// - chama geneerator_password() quando escolher gerar senha
class Presentation {
public:
    Presentation();
    ~Presentation();

    // Loop principal da apresentação (menu)
    void run();

private:
    bool ensureReady();   // inicializa DB e repository
    void printHeader() const;
    void printMenu() const;

    // handlers das opções
    void handleGenerate();
    void handleSave();    
    void handleFetch();
    // utilidades
    static void clearStdin();
    static std::string readLine(const char* prompt);

    PasswordModel m_model;
    std::unique_ptr<PasswordRepository> m_repo;
    mutable QSqlError m_lastError;
};
