if [ "$1" -eq 0 ]; then
    ffplay -v 0 -nodisp -autoexit /home/andrew/Data/Work/UniversalDevice/.vscode/VSCode_OK.mp3
    wait
else
    ffplay -v 0 -nodisp -autoexit /home/andrew/Data/Work/UniversalDevice/.vscode/VSCode_error.mp3
    wait
fi
