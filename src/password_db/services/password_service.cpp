#include "password_service.h"

PasswordService::PasswordService(PasswordRepository& repo)
    : m_repo(repo) {}

bool PasswordService::save(const QString& masterPassword,
                           const QString& name,
                           const QString& plaintext) {
    if (name.isEmpty() || masterPassword.isEmpty() || plaintext.isEmpty())
        return false;

    // Salt por registro + IV
    QByteArray salt = cr_randomBytes(16);
    QByteArray iv   = cr_randomBytes(12);
    QByteArray key  = cr_deriveKey(masterPassword, salt, 32, 100000);

    // Criptografa
    QByteArray ct, tag;
    if (!cr_encrypt_aes_gcm(key, plaintext.toUtf8(), iv, ct, tag))
        return false;

    // Hash do plaintext (para verificação/integração futura)
    const QString hashHex = cr_sha256_hex(plaintext.toUtf8());

    PasswordEntity e;
    e.name        = name;
    e.cipher      = ct;
    e.iv          = iv;
    e.salt        = salt;
    e.tag         = tag;
    e.secret_hash = hashHex;

    // upsert
    return m_repo.upsert(e);
}

std::optional<QString> PasswordService::fetch(const QString& masterPassword,
                                              const QString& name) {
    if (name.isEmpty() || masterPassword.isEmpty())
        return std::nullopt;

    auto e = m_repo.get(name);
    if (!e.has_value())
        return std::nullopt;

    QByteArray key = cr_deriveKey(masterPassword, e->salt, 32, 100000);
    QByteArray plain;
    if (!cr_decrypt_aes_gcm(key, e->iv, e->cipher, e->tag, plain))
        return std::nullopt; // master errada ou dados corrompidos

    // (Opcional) checar integridade via hash
    const QString computed = cr_sha256_hex(plain);
    if (!e->secret_hash.isEmpty() && computed != e->secret_hash)
        return std::nullopt;

    return QString::fromUtf8(plain);
}

bool PasswordService::remove(const QString& name) {
    if (name.isEmpty()) return false;
    return m_repo.remove(name);
}
