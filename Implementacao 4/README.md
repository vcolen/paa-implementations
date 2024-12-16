
# Projeto de Segmentação de Imagens com Algoritmos Baseados em Grafos

Este projeto implementa dois métodos de segmentação de imagens baseados em grafos:
1. **Algoritmo de Felzenszwalb-Huttenlocher (FH)**.
2. **Algoritmo de Corte Mínimo (Boykov-Funka-Lea)**.

___

## Integrantes

Camila Lopes: Responsável pela confecção do artigo e revisão do código.
Victor Colen: Responsável pela confecção do código e revisão do artigo.

---

## Pré-requisitos

1. **Sistema operacional**: macOS.
2. **Ferramentas de desenvolvimento**:
   - Xcode Command Line Tools: necessário para compilar o código.
   - OpenCV: biblioteca para manipulação de imagens.

---

## Instalação

### 1. Instale o Homebrew (se ainda não tiver instalado)

Abra o Terminal e execute o comando:

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

Verifique a instalação:

```bash
brew --version
```

---

### 2. Instale o OpenCV

Use o Homebrew para instalar o OpenCV:

```bash
brew install opencv
```

---

### 3. Configure o ambiente para OpenCV

Adicione o caminho do OpenCV ao compilador. No Terminal, rode o comando:

```bash
export PKG_CONFIG_PATH=/opt/homebrew/lib/pkgconfig
```

Para tornar essa configuração permanente, adicione o comando acima ao arquivo `~/.zshrc` ou `~/.bash_profile` (dependendo do shell que você usa):

```bash
echo "export PKG_CONFIG_PATH=/opt/homebrew/lib/pkgconfig" >> ~/.zshrc
source ~/.zshrc
```

Verifique se o OpenCV está acessível:

```bash
pkg-config --modversion opencv4
```

---

## Compilando o Projeto

Certifique-se de que os arquivos do projeto estão no mesmo diretório. Para compilar o código:

1. Navegue até o diretório do projeto no Terminal:

```bash
cd /paa-implementations/Implementacao\ 4/
```

2. Compile os arquivo usando o comando:

```bash
g++ -std=c++11 -I/opt/homebrew/include/opencv4 -L/opt/homebrew/lib FH.cpp -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_imgproc -o FH`
g++ -std=c++11 -I/opt/homebrew/include/opencv4 -L/opt/homebrew/lib BF.cpp -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_imgproc -o BF`
```

3. O comando acima gera dois executáveis chamados `FH` e `BF`.

---

## Executando o Projeto

1. Execute o programa:

```bash
./FH
```
ou
```bash
./BF
```

3. O programa exibirá as imagens originais e segmentadas. Pressione qualquer tecla para avançar para a próxima imagem ou encerrar.

---

## Referências

- **OpenCV**: [Documentação oficial](https://opencv.org/)
- **Homebrew**: [Site oficial](https://brew.sh/)
