#pragma once
#include <QString>
#include <optional>

#include <repositories/repository.h>
#include "crypt.h"

// Serviço de senhas: aplica hash/crypto e usa o PasswordRepository para CRUD.
// NÃO toca em schema.
class PasswordService {
public:
    explicit PasswordService(PasswordRepository& repo);

    // Salva/atualiza uma senha em claro:
    // - Gera salt (registro), iv e tag
    // - Deriva chave a partir da masterPassword + salt do registro
    // - Criptografa (AES-256-GCM)
    // - Gera secret_hash = SHA256(plaintext) em hex
    bool save(const QString& masterPassword,
              const QString& name,
              const QString& plaintext);

    // Busca e decripta. Retorna std::nullopt se não existir ou master inválida.
    std::optional<QString> fetch(const QString& masterPassword,
                                 const QString& name);

    // Deleta por name
    bool remove(const QString& name);

private:
    PasswordRepository& m_repo;
};
