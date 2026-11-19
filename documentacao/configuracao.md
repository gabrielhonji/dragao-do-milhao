# Configuração do projeto

Abra no VS Code a pasta que contém o `Makefile` com **File → Open Folder**. Instale a extensão **C/C++**, da Microsoft (`ms-vscode.cpptools`), em **Extensions**. A mesma extensão oferece IntelliSense e executa o clang-format.

## Compilar no Windows

1. Instale o [MSYS2](https://www.msys2.org/) e abra **MSYS2 UCRT64**. Rode `pacman -Syu`. Se a atualização pedir para fechar o terminal, reabra-o e repita o comando.
2. Pelo terminal **MSYS2 UCRT64**, instale o compilador, o Make e a raylib:

   ```bash
   pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make mingw-w64-ucrt-x86_64-raylib
   ```

3. Adicione `C:\msys64\ucrt64\bin` ao `Path` do usuário e reabra o VS Code. Ajuste o caminho se instalou o MSYS2 em outra pasta.
4. No terminal PowerShell do VS Code, compile e execute a partir da raiz:

   ```powershell
   mingw32-make
   .\dragao-do-milhao.exe --visual
   ```

   Execute sem `--visual` para abrir o cadastro pelo terminal.

## Compilar no Linux

Instale GCC, GNU Make e as dependências de desenvolvimento de OpenGL e X11 da sua distribuição. O projeto inclui os cabeçalhos e a biblioteca da raylib usados pelo Makefile para Linux.

```bash
make
./dragao-do-milhao --visual
```

## Formatação e documentação

Salvar um arquivo reconhecido como **C** no VS Code aplica a configuração de `.clang-format`. As configurações ficam em `.vscode/settings.json`. Se um cabeçalho `.h` aparecer com outro modo de linguagem, selecione **C** no canto inferior direito do editor.

Use português em comentários, nomes de funções e variáveis do projeto. Em identificadores e nomes de arquivos, use palavras sem acentos e separe-as com sublinhados. Preencha o `@brief` dos arquivos com uma frase que descreva sua responsabilidade. Mantenha a autoria existente ao editar.

A fonte Pixelify Sans está em `assets/fontes/PixelifySans.ttf`, acompanhada de `OFL.txt`.
