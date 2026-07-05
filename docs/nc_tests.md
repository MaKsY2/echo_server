# Через nc решил выжать максимум и проверить че можем 

`Запускал через:`
```
taskset -c 2 bash -c 'yes "ping" | head -100000 | nc -N localhost 8080 > /dev/null'
```

`Итог:`

```
[pussy_destroyer@taskrunner echo_server]$ taskset -c 3 ./build/echo_server
listening on :8080
client connected (fd=4)
server-side echo (126 samples), ns:
  p50    = 1044
  p99    = 9260
  p99.9  = 10708
  max    = 24778
client disconnected
```

Сначала оч удивился почему 126 всего, оказалось что там все иначе работает, нежели 100к TCP, там пока обрабатывается один пакет, копится буффер 4кб (который я и установил)

```c
std::array<std::byte, 4096> buf{};
```

126 все еще маленькая выборка, дальше делать только свой нагрузчик

