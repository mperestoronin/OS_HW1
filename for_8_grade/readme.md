# Отчет о работе программы:
### небольшой гайд по запуску:
для запуска написать следующие команды в терминале:
- mkfifo pipe1
- mkfifo pipe2
- mkfifo sync_pipe
это создаст pipe1, pipe2, sync_pipe
после чего пишем ./main8 в одном окне терминала, а в другом ./main8rw input.txt output.txt
