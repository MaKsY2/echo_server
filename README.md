[![CI](https://github.com/MaKsY2/echo_server/actions/workflows/ci.yaml/badge.svg?branch=master)](https://github.com/MaKsY2/echo_server/actions/workflows/ci.yaml)

## features
- thread-per-connection
- конченный cplusplus 20

## future feature
- epoll
- io_uring
- etc...

## gtest
- интеграционная шляпа 
- интеграционная шляпа, но с параллельными запросами



# Тестовый стенд
Lenovo ThinkPad T580:
- 4 ядра, 8 потоков 
- убрал турбо, все ядра теперь 400-1600 (что обидно)
- запинил ядра, 2,3 6,7 теперь не получают нагрузку от ОС, буду там тесты гонять, 2-3 нагрузчик, 6-7 эхо сервер
    - по два, потому что они логические....
- перевел в режим перфоманс через cstate

## Итоги тестовых замеров:

Первый прогон на 3 ядре 

```
[pussy_destroyer@taskrunner utils]$ taskset -c 3 ./noise_floor
noise floor, ns:
  p50    = 36
  p99    = 39
  p99.9  = 42
  p99.99 = 44
  max    = 583
```

Второй прогон на 3 ядре 

```
[pussy_destroyer@taskrunner utils]$ taskset -c 3 ./noise_floor
noise floor, ns:
  p50    = 36
  p99    = 39
  p99.9  = 39
  p99.99 = 44
  max    = 680
```

Третий прогон на 3 ядре 
```
[pussy_destroyer@taskrunner utils]$ taskset -c 3 ./noise_floor
noise floor, ns:
  p50    = 36
  p99    = 39
  p99.9  = 41
  p99.99 = 44
  max    = 686
```

Прогон на нулевом ядре (отдан ОС)
```
[pussy_destroyer@taskrunner utils]$ taskset -c 0 ./noise_floor
noise floor, ns:
  p50    = 36
  p99    = 39
  p99.9  = 43
  p99.99 = 62
  max    = 8160
```


p50 - 36нс, увы, но это минимум, далее будем судить уже от этого числа,
Машина если че Kaby Lake @ 1.6 GHz, 58 тактов где-то я считал 

ну и p99.9 у ядра с операционнкой показывает, что иногда мы в прерываниях обмазались 



Бахнул