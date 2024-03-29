# Отчет о работе программы:
### Небольшой гайд по запуску:
для запуска написать следующие команды в терминале:
- `mkfifo pipe1`
- `mkfifo pipe2`
- `mkfifo sync_pipe`
это создаст `pipe1, pipe2, sync_pipe`
после чего пишем `./main8` в одном окне терминала, а в другом `./main8rw input.txt output.txt`
### Непосредственно про саму программу:
Эта программа представляет собой простой пример межпроцессного взаимодействия между двумя программами, работающими одновременно.
Программа состоит из двух отдельных частей, каждая из которых предназначена для запуска одновременно с другой. Одна часть программы считывает входной текстовый файл и записывает его содержимое в канал, в то время как другая часть считывает из канала, подсчитывает количество знаков препинания и записывает результат в другой канал.

Две части программы взаимодействуют друг с другом, используя два именованных канала и канал синхронизации. Канал синхронизации используется для того, чтобы передать процессу, подсчитывающему знаки препинания, что входной файл уже был загружен в канал (тем самым запустить его).

Первая часть программы начинается с ожидания сигнала от канала синхронизации, указывающего на то, что вторая часть программы готова начать обработку данных. Как только сигнал получен, он открывает первый именованный канал для записи и второй именованный канал для чтения, а затем начинает считывать входной текстовый файл. Когда он считывает данные из входного файла, он записывает их в первый именованный канал. Как только весь входной файл был прочитан и записан в канал, эта часть программы закрывает канал и завершает работу.

Вторая часть программы начинается с ожидания сигнала синхронизации по тому же каналу. Как только сигнал получен, он открывает первый именованный канал для чтения и второй именованный канал для записи. Затем он перенаправляет свои стандартные входные и выходные потоки в первый и второй каналы соответственно, используя системный вызов `dup2()`. Наконец, он запускает внешнюю программу под названием punctuation_counter (находится в отдельном файле, как и в других программах), используя системный вызов `execlp()`. Эта внешняя программа принимает входные данные из своего стандартного входного потока (который теперь является первым именованным каналом) и записывает свои выходные данные в свой стандартный выходной поток (который теперь является вторым именованным каналом).

По критериям надо указать имена потоков, так что вот:
``` С
#define PIPE1_NAME "pipe1"
#define PIPE2_NAME "pipe2"
#define SYNC_PIPE_NAME "sync_pipe"
```
Наконец, вторая часть программы считывает выходные данные из второго именованного канала и записывает их в выходной файл. Как только все выходные данные будут прочитаны и записаны, эта часть программы закрывает два именованных канала и завершает работу.
