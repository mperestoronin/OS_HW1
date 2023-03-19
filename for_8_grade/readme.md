для запуска написать 
mkfifo pipe1
mkfifo pipe2
mkfifo sync_pipe
это создаст pipe1, pipe2, sync_pipe
после чего запускаем ./perest8 в одном окне терминала, а в другом ./perest8rw input.txt output.txt
