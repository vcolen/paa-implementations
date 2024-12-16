
# Projeto de Segmentação de Imagens com Algoritmos Baseados em Grafos

Este projeto implementa dois métodos de segmentação de imagens baseados em grafos:
1. **Algoritmo de Felzenszwalb-Huttenlocher (FH)**.
2. **Algoritmo de Corte Mínimo (Boykov-Funka-Lea)**.

Os métodos são usados para particionar imagens com base em suas propriedades visuais, como cor e intensidade.

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
cd /caminho/para/o/diretorio/do/projeto
```

2. Compile o arquivo usando o comando:

```bash
g++ -std=c++17 -o segmentacao main.cpp `pkg-config --cflags --libs opencv4`
```

3. O comando acima gera um executável chamado `segmentacao`.

---

## Executando o Projeto

1. Coloque as imagens de entrada no mesmo diretório do executável e garanta que os nomes das imagens correspondam aos definidos no código.

2. Execute o programa:

```bash
./segmentacao
```

3. O programa exibirá as imagens originais e segmentadas. Pressione qualquer tecla para avançar para a próxima imagem ou encerrar.

---

## Estrutura do Projeto

- **main.cpp**: Arquivo principal contendo a implementação dos algoritmos.
- **Imagens de entrada**: Arquivos `.jpeg` ou `.jpg` que serão segmentados.

---

## Problemas Comuns

### 1. OpenCV não encontrado
Certifique-se de que o caminho do OpenCV foi configurado corretamente no `PKG_CONFIG_PATH`.

### 2. Erro ao carregar imagens
Garanta que os arquivos de imagem estejam no diretório correto e que os nomes correspondam ao especificado no código.

---

## Referências

- **OpenCV**: [Documentação oficial](https://opencv.org/)
- **Homebrew**: [Site oficial](https://brew.sh/)
