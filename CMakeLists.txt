# Define a versão mínima do CMake necessária para o projeto
cmake_minimum_required(VERSION 3.10)

# Define o nome do projeto
project(GeradorDeSenha VERSION 1.0)

# Ativa o suporte ao C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define as pastas que contêm os arquivos de cabeçalho
include_directories(
    src/generate_password
)

# Adiciona um executável chamado "gerador_de_senha"
# e lista todos os arquivos .cpp que serão compilados para criá-lo
add_executable(gerador_de_senha
    src/generate_password/main.cpp
    src/generate_password/generator_password.cpp
)