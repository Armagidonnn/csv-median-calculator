# CSV Median Calculator

## Описание

Консольное приложение на C++23 для обработки CSV-файлов с ценовыми данными.

Программа считывает CSV-файлы из указанной директории, сортирует записи по времени получения (`receive_ts`) и вычисляет инкрементальную медиану цены.

Результат сохраняется в выходной CSV-файл.

---

## Функциональность

- чтение CSV-файлов из директории
- фильтрация файлов по маске
- сортировка записей по receive_ts
- инкрементальный расчёт медианы
- запись результата только при изменении медианы
- логирование процесса обработки

---

## Используемые технологии

- C++23
- CMake ≥ 3.23
- Boost.Program_options
- toml++
- spdlog
- std::filesystem

---

## Формат входных данных

CSV с разделителем `;`

Пример:

receive_ts;price  
1700000001;100.25

---

## Формат конфигурации (config.toml)

```toml
input_dir = "examples/input"
output_dir = "examples/output"
filename_mask = "*.csv"
```

---

## Сборка

```bash
cmake -S . -B build
cmake --build build --config Release
```

---

## Запуск (PowerShell / Windows)

```powershell
.\build\Release\csv_median_calculator.exe -c config.toml
```

Если конфиг расположен рядом с приложением:

```powershell
.\build\Release\csv_median_calculator.exe
```

---

## Результат работы

В выходной директории создаётся файл:

median_result.csv

Формат:

receive_ts;price_median

---

## Обработка ошибок

- некорректные строки CSV пропускаются
- ошибки логируются через spdlog
- выполнение программы не прерывается

---

## Возможные улучшения

- потоковая (streaming) обработка CSV
- epsilon-сравнение медианы
- unit-тесты
- многопоточная обработка файлов
