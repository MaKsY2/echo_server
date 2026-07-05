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

