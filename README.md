# Kplet accuracy test
Check proposed accuracy for kplet algorithm, described in the article [habr.com](https://habr.com/ru/companies/samsung/articles/842578/),
with ippm descriptor

![MinexIII databse like, accuracy test.](https://github.com/ilia-isaev/biometrics-ippmd-kplet/blob/main/kplet_minexiii_accuracy.jpg)

## Prepare
```
autoreconf -i
./autogen.sh
```

## Build
```
./configure
make
```

## Usage
```
./src/kplet t1.incits378 t2.incits378 tag
```
Were "tag" is label to mark genuine and impostor.
Example 0 - impostor, 1 - genuine template pair
