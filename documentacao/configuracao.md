# Como rodar o jogo

Abra a pasta do projeto no VS Code e instale a extensão **C/C++**, da Microsoft. Depois é só salvar o arquivo C. O VS Code formata o código usando a configuração do projeto.

A extensão **Doxygen Documentation Generator** monta o comentário de uma função quando você digita `/**` na linha acima dela e pressiona **Enter**.

Depois de instalar o compilador, abra **Terminal → Run Task...** e escolha **Abrir jogo no modo visual**.

## Windows

Instale o [MSYS2](https://www.msys2.org/) e abra o terminal **MSYS2 UCRT64**. Rode:

```bash
pacman -Syu
pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make mingw-w64-ucrt-x86_64-raylib
```

Se a atualização pedir para fechar o terminal, abra o **MSYS2 UCRT64** novamente e repita a atualização antes de instalar os pacotes.

Adicione `C:\msys64\ucrt64\bin` ao `Path` do usuário e reabra o VS Code. No PowerShell, dentro da pasta do projeto:

```powershell
mingw32-make visual
```

Para abrir o cadastro de perguntas no terminal, use `mingw32-make run`.

## Linux

Com GCC, Make, OpenGL e X11 instalados, rode na pasta do projeto:

```bash
make visual
```
