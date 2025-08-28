# Versão mínima do CMake
cmake_minimum_required(VERSION 3.16)

# Projeto
project(GeradorDeSenha VERSION 1.0 LANGUAGES CXX)

# C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt6 módulos necessários
find_package(Qt6 COMPONENTS Core Gui Qml Quick QuickControls2 Sql REQUIRED)

# OpenSSL (Crypto)
find_package(OpenSSL REQUIRED)

# MOC / RCC automáticos
set(CMAKE_AUTOMOC ON)
# Você pode usar AUTORCC OU qt_add_resources. Recomendo qt_add_resources (abaixo), então AUTORCC é opcional.
# set(CMAKE_AUTORCC ON)

# Fontes C++
add_executable(gerador_de_senha
    src/main.cpp

    # Models / DB
    src/password_db/models/model.cpp

    # Repositories
    src/password_db/repositories/repository.cpp
    src/password_db/repositories/user_repository.cpp

    # Services
    src/password_db/services/password_service.cpp
    src/password_db/services/crypt_service.cpp
    src/password_db/services/auth_service.cpp

    # Gerador de senha
    src/generate_password/generator_password.cpp

    # QMVC
    src/qmvc/password_list_model.cpp
    src/qmvc/controller.cpp
)

# Recursos (QML embutidos)
qt_add_resources(QML_RES
    "resources.qrc"
)
target_sources(gerador_de_senha PRIVATE ${QML_RES})

# Includes do projeto (apenas diretórios com headers C++; NÃO coloque QML aqui)
target_include_directories(gerador_de_senha PRIVATE
    src
)

# Link com os módulos do Qt e OpenSSL
target_link_libraries(gerador_de_senha PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Sql
    OpenSSL::Crypto
)
