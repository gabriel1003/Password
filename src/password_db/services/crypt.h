#pragma once
#include <QString>
#include <QByteArray>

// Bloco criptografado (AES-256-GCM)
struct EncBlob {
    QByteArray salt;       // 16B, PBKDF2 salt (por registro)
    QByteArray iv;         // 12B, GCM nonce
    QByteArray tag;        // 16B, GCM tag
    QByteArray ciphertext; // dados criptografados
};

// Utils
QByteArray cr_randomBytes(int len);

// PBKDF2-HMAC-SHA256(master, salt) -> keyLen bytes (32 = AES-256)
QByteArray cr_deriveKey(const QString& master, const QByteArray& salt,
                        int keyLen = 32, int iterations = 100000);

// SHA-256 em hex minúsculo
QString cr_sha256_hex(const QByteArray& data);

// AES-256-GCM
bool cr_encrypt_aes_gcm(const QByteArray& key,
                        const QByteArray& plaintext,
                        const QByteArray& iv,        // 12B
                        QByteArray& ciphertextOut,
                        QByteArray& tagOut);         // 16B

bool cr_decrypt_aes_gcm(const QByteArray& key,
                        const QByteArray& iv,        // 12B
                        const QByteArray& ciphertext,
                        const QByteArray& tag,
                        QByteArray& plaintextOut);
