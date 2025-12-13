# Engine-Wrapper лабораторная работа

## Table of Contents //TODO

- [Что такое ArgList](#ArgList)
- [Wrapper](#Wrapper)
    - [Описание конструкторов](#все-конструкторы-класса-wrapper-принимают)
    - [`execute()`](#execute)
    - [Примеры](#examples)
- [Engine](#engine)
  - [Что он может](#что-он-может)
  - [Примеры](#примеры)
- [Сборка и запуск](#сборка-и-запуск)

---

## `ArgList`

### `using ArgList = std::vector<std::pair<std::string, std::any>>`

### Возможный синтаксис:

1. Пустой: `{}`
2. 1 аргумент: `{{"a", 5}}`
3. n аргументов: `{{"a", 6}, {...}, ..., {"z", 7}}`
4. Можно просто использовать свой вектор

---

## `Wrapper`

### Все конструкторы класса `Wrapper` принимают:

1. Объект, указатель на объект, `unique_ptr` или `shared_ptr` на объект
2. Ссылку на функцию (должна быть функцией объекта, статичные, константные, `volatile` функции не поддерживаются)
3. `ArgList` - `{}` (empty) or `{{"a", 5}}` (первый аргумент функции теперь называется `a` и имеет дефолтное значение
   `5`). Количество аргументов в ArgList должно точно совпадать с количеством аргументов функции, так как на этом этапе
   мы даем им внутреннее имя, которое в дальнейшем будет использоваться при вызове функции

### `execute()`

- Принимает `ArgList`. Его размер должен быть не больше количества аргументов функции, а типы аргументов должны
  совпадать (Если `a` раньше было `int`, то сейчас тоже должно быть `int`. Кастинг не поддерживается)
- Сопоставляет аргументы по именам. Если в функции не передали какой-то аргумент, используется его дефолтное (указанное
  при конструировании) значение

### Examples:

#### Какой-то класс `Subject`:

```c++
class Subject {
public:
    int double_it(int x) { return x * 2; }
    double sum(double a, double b) { return a + b; }
    void set_sum(int arg1, int arg2) { last_called = arg1 + arg2; }
    std::string concat_num_to_string(const std::string& s, int n) { return s + std::to_string(n); }

    int last_called = 0;
};
```

#### Базовые use-case:

1. `execute()` без аргументов:

```c++
    Subject subj;
    Wrapper wrapper(subj, &Subject::double_it, {{"x", 10}});
    auto result = wrapper.execute({});
    std::any_cast<int>(result); //== 10 * 2
```

2. `execute()` с аргументами:

```c++
    Subject subj;
    Wrapper wrapper(subj, &Subject::double_it, {{"x", 0}});
    auto result = wrapper.execute({{"x", 7}});
    std::any_cast<int>(result); // == 7 * 2
```

#### Все варианты использования конструктора:

1. Копирование объекта внутрь (по `const ref`)

```c++
    Subject subj;
    Wrapper wrapper(subj, &Subject::set_sum, {{"arg1", 0}, {"arg2", 1}}); //copies subj internally
    auto result = wrapper.execute({{"arg1", 42}});
    result.has_value(); //is false due to function returns void
    subj.last_called; // == 0 as original object is not changed
```

2. Raw ptr. Будьте осторожны, очень велика вероятность нарваться на segfault, если `execute()` будет после того, как объект умрет. Используйте на свой страх и риск! Smart pointerы предпочтительнее.

```c++
    auto* subj = new Subject();
    Wrapper wrapper(subj, &Subject::set_sum, {{"arg1", 0}, {"arg2", 1}}); //just stores raw ptr
    auto result = wrapper.execute({{"arg1", 42}});
    result.has_value(); //still false
    subj->last_called; // == 43,  original object is changed
    delete subj; //after deleting executing wrapper is UB, likely SEGFAULT, as raw pointer is just stored inside Wrapper
```

3. Unique ptr

```c++
    auto subj = std::make_unique<Subject>();
    Subject* observer = subj.get();

    Wrapper wrapper(std::move(subj), &Subject::set_sum, {{"arg1", 0}, {"arg2", 1}}); //unique ptrs have to be moved
    //subj is now nullptr
    auto result = wrapper.execute({{"arg1", 42}});
    result.has_value(); //still false
    observer->last_called; // == 43, original object is changed
```

4. Shared ptr

```c++
    std::shared_ptr<Subject> subj = std::make_shared<Subject>();
    Wrapper wrapper(subj, &Subject::set_sum, {{"arg1", 0}, {"arg2", 1}}); //copies shared ptr inside. Can be moved tho
    auto result = wrapper.execute({{"arg1", 42}});
    result.has_value(); //still false
    subj->last_called; // == 43,  original object is changed
```

---

## Engine
Регистр зарегистрированных команд, которые можно запускать

### Что он может:
1. Зарегистрировать команду: `Engine.register_command("name", wrapper)`
2. In-place создать `Wrapper` и зарегистрировать команду: `Engine.register_command("name", subj, Subject::sum, {{"a", 0}, {"b", 0}})` - поддерживает все конструкторы Wrapper'a (`subj` может быть raw ptr, const ref, unique ptr or shared ptr)
3. Запускать команды: `Engine.execute("name", {/*args here*/})`


Если команда не была зарегистрирована, выкинет `std::invalid_argument`
Все ошибки, которые возникнут внутри `Wrapper`, в нем не обрабатываются и передаются дальше

### Примеры:
```c++
    Engine engine;
    Subject subj;
    Wrapper wrapper(subj, &Subject::double_it, {{"x", 0}});
    engine.register_command("double_it", wrapper);

    auto result = engine.execute("double_it", {{"x", 5}});
    std::any_cast<int>(result); // 5*2
```

---

## Tests
Еще больше примеров и исходники тестов можно найти в папке [тестов](tests)

## Сборка и запуск
### Сконфигурируйте CMake:
`cmake -S ./ -B ./build` (`-G Ninja` по желанию)

### Сборка и запуск:
1. Сборка: `cmake --build ./build`
2. Запуск основной программы ([main.cpp](main.cpp)): `./build/EngineWrapper` - должно вывести 16
3. Запуск тестов: `./build/tests/EngineWrapperTests` - тут GTests
