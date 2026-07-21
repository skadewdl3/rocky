@echo off
rem Wrapper so `git clang-format` works from native Windows (cmd/PowerShell),
rem where shebang lines aren't honored. Git for Windows finds this via
rem PATHEXT when resolving the `git-clang-format` subcommand. On Linux/macOS/
rem WSL/Git Bash, the sibling `git-clang-format` file's shebang is used
rem instead, and this .cmd file is simply ignored.
where python3 >nul 2>nul
if %errorlevel%==0 (
    python3 "%~dp0git-clang-format" %*
) else (
    python "%~dp0git-clang-format" %*
)
