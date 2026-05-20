# Block Blast - C ve Raylib

Bu proje, C ve raylib ile yazılmış basit bir Block Blast klonudur. Oyun kodunun tamamı tek dosyada tutulur:

```text
main.c
```

`raylib/` klasörü projeyle gelen harici kütüphanedir.

## Derleme ve Çalıştırma

Gerekenler:

- `gcc` veya `clang`
- `make`

### 1. Raylib'i Derle

Raylib'i her seferinde yeniden derlemek gerekmez. Projeyi ilk kez aldıktan sonra bir kere şu komutu çalıştır:

```bash
make -C raylib/src PLATFORM=PLATFORM_DESKTOP RAYLIB_LIBTYPE=STATIC RAYLIB_BUILD_MODE=RELEASE
```

Bu komut `raylib/src/libraylib.a` dosyasını oluşturur.

### 2. Oyunu Derle

macOS:

```bash
gcc -std=c99 -Wall -Wno-missing-braces -Iraylib/src main.c raylib/src/libraylib.a -framework OpenGL -framework Cocoa -framework IOKit -framework CoreAudio -framework CoreVideo -o blockblast
```

Linux:

```bash
gcc -std=c99 -Wall -Wno-missing-braces -Iraylib/src main.c raylib/src/libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11 -o blockblast
```

Windows (MinGW):

```bat
gcc -std=c99 -Wall -Wno-missing-braces -Iraylib\src main.c raylib\src\libraylib.a -lopengl32 -lgdi32 -lwinmm -o blockblast.exe
```

Çalıştırma:

```bash
./blockblast
```

## Proje Yapısı

```text
main.c      Tüm oyun kodu: sabitler, tipler, oyun mantığı, input, render, ses, kayıt ve main loop
assets/         Font, görsel ve ses dosyaları
raylib/         Projeyle gelen raylib kaynakları
data/           Çalışma sırasında oluşan kişisel kayıt dosyaları
```

## Kayıt Dosyası

Oyun açılırken `data/` klasörü oluşturulur. Bu klasör `.gitignore` içindedir.

Kayıt binary değil, okunabilir tek bir metin dosyasıdır:

```text
data/save.txt
```

Örnek:

```text
highScore=1200
unlockedLevel=8
```

`unlockedLevel=8`, 1-7 arası adventure level'ların geçildiği ve 8. level'ın açık olduğu anlamına gelir. `unlockedLevel=11` tüm 10 adventure level'ının tamamlandığını gösterir.

## Notlar

- Scoreboard ve nickname özellikleri yoktur.
- Adventure mode durur, ancak level hedefleri `main.c` içinde sabittir.
- Raylib kaynaklarını her oyun derlemesinde yeniden derlememek için `libraylib.a` kullanılır.