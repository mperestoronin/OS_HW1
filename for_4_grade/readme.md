### Описаниие работы программы:
Сначала программа проверяет, что ей передали ровно два аргумента командной строки
(имена входного и выходного файлов). Если количество аргументов неверно,
программа выводит сообщение об использовании и завершает работу.
Два неименнованных канала создаются с помощью `pipe()`.
Эти каналы используются для обмена данными между родительским процессом и его дочерними процессами.
Три дочерних процесса создаются с помощью `fork()`.
Каждый дочерний процесс выполняет свою функцию, как описано ниже.

Первый дочерний процесс (child1) отвечает за чтение входного файла и запись его содержимого в первый канал.
Это делается путем вызова функции `read_from_file`.

Второй дочерний процесс (child2) считывает данные из первого канала и обрабатывает их с помощью программы под названием punctuation_counter (представлено в отедльном файле).
Для этого вызывается функция execute_punctuation_counter.
Третий дочерний процесс (child3) считывает данные из второго канала и записывает результаты в выходной файл.
Это делается через функцию `write_to_file`.

После создания дочерних процессов родительский процесс закрывает все неиспользуемые каналы и ожидает завершения дочерних процессов с помощью `waitpid`.



