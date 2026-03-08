# Proje: C ve Raylib ile Block Blast Klonu

## Proje Tanımı ve Hedefleri

Bu proje, Block Blast oyununun C ve Raylib kütüphanesi kullanılarak geliştirilmiş klonudur. Temel amaç, bellek yönetimi ve pointer manipülasyonu gibi düşük seviyeli yetkinlikleri oyun geliştirme dinamikleriyle birleştirerek modüler bir yapı ortaya koymaktır. Projede harici kütüphane kullanımı yasaktır.

## Kurulum ve Derleme

Bu proje **CMake** ve **Raylib** kullanılarak geliştirilmektedir. Raylib bağımlılığı CMake üzerinden otomatik olarak (`FetchContent` ile) Github'dan indirilir.

### Ön Koşullar
Projeyi bilgisayarınızda derleyip çalıştırabilmek için aşağıdaki araçların yüklü olması zorunludur:
* **Git:** Raylib kütüphanesini arka planda indirebilmek için ([İndir](https://git-scm.com/downloads)).
* **C/C++ Derleyicisi (Compiler):** 
  * **Windows:** Visual Studio C++ Build Tools (MSVC) veya MinGW/GCC.
  * **macOS:** Xcode Command Line Tools (`xcode-select --install` terminal komutu ile).
  * **Linux:** GCC veya Clang (`sudo apt install build-essential` komutu ile).
* **CMake:** Derleme sürecini yönetmek için ([İndir](https://cmake.org/download/)).

### VS Code ile Çalıştırma
1. Cihazınızda Git, CMake ve uygun bir derleyici kurulu olduğundan emin olun.
2. Bu projeyi VS Code ile açın.
3. VS Code'un **C/C++** ve **CMake Tools** eklentilerini yükleyin.
4. Alt durum çubuğunda (veya komut paletinde `CMake: Select a Kit` yazarak) doğru **Derleyici Kitini** (Örn: Windows için *Visual Studio Community 2022 Release - amd64* veya *GCC*) seçin.
5. `Build` (Derle) butonuna basarak projenin derlenmesini bekleyin. Derleme başarılı olduktan sonra `Play` (Çalıştır) butonuna tıklayarak oyunu başlatabilirsiniz.

## Teknik Kısıtlar ve Mimari

* **Veri Yapıları:** Oyun elemanlarının struct yapıları ile tanımlanması.
* **Hafıza Yönetimi:** malloc/free ile dinamik bellek kontrolü.
* **Algoritmalar:** Çok boyutlu diziler, döngüler ve koşullu ifadeler.
* **Pointerlar:** Fonksiyonlar arası veri aktarımı ve bellek manipülasyonu.
* **Dosya Yönetimi (I/O):** Skor tablosunun ve durumun kaydedilmesi.
* **Animasyon:** Sadece ham kod ve Raylib'in yerleşik zamanlayıcısı (GetFrameTime) ile timer mantığı kurulması.

## Modüler Ayrıştırma

* **main.c:** Ana oyun döngüsü ve durum makinesi (Menü, Oyun, Bitiş).
* **grid.c / grid.h:** Çok boyutlu dizi yönetimi, blok yerleştirme ve temizleme algoritmaları.
* **shapes.c / shapes.h:** Blok yapıları, bellek tahsisi ve rastgele üretim mantığı.
* **input.c / input.h:** Sürükle-bırak kontrolleri ve grid koordinat dönüşümleri.
* **visuals.c / visuals.h:** Raylib texture render işlemleri, UI çizimleri.
* **save.c / save.h:** Yüksek skor File I/O işlemleri.

## Mantıksal Akış

* **Çarpışma Algılaması:** Sürüklenen matris ile hedef ızgara matrisinin pointer üzerinden çakışma testi.
* **Izgara Temizleme ve Kombo:** Satır/sütun taraması. Puan = Taban x N x Kombo formülü ile skor hesabı. (N: Eşzamanlı temizlenen hat sayısı).
* **Oyun Bitiş:** Mevcut blokların boşluklara sığmama durumunun arka planda simüle edilmesi.

## Teslimat ve Değerlendirme

Teslim edilecek materyaller: Kaynak kodlar, proje raporu, maksimum 5 dakikalık demo videosu ve canlı sunum dosyası. Her üye kodun tamamına hakim olmak zorundadır.

| Kriter | Ağırlık |
| --- | --- |
| Zorunlu Programlama Konseptlerinin Kullanımı | %25 |
| Sözlü Sunum ve Jüri Performansı | %25 |
| Kod Kalitesi ve Doğruluğu | %20 |
| Yaratıcılık ve Oyun Tasarımı | %15 |
| Rapor ve Dokümantasyon | %10 |
| Video Sunumu | %5 |

## Takım To-Do List

### 1. **02 - 09 Mart | Kişisel Tanıtım Sayfası**

* **Hedef:** Local ve Remote repository bağlantısını kurmak.
* **Görev:** Kendinizi tanıtan bir `README.md` dosyası oluşturun.
* **Kazanımlar:**
* Dosya takibi: `git init`, `git add`, `git commit`
* Buluta gönderim: `git remote add`, `git push`

### 2. **09 - 16 Mart | Basit Hesap Makinesi**

* **Hedef:** Git "Branch" (Dal) yapısını öğrenmek.
* **Görev:** Ana dalda (main) toplama, yeni bir dalda (`feature-subtraction`) çıkarma özelliğini geliştirin.
* **Kazanımlar:**
* Dal yönetimi: `git branch`, `git checkout`, `git switch`
* Birleştirme: `git merge`

### 3. **16 - 23 Mart | İstatistik Kütüphanesi**

* **Hedef:** Kodu modüler parçalara bölmek.
* **Görev:** Sayı dizisinin ortalamasını ve maksimumunu bulan sistemi 3 dosyaya ayırın (`math_utils.h`, `math_utils.c`, `main.c`).
* **Kazanımlar:**
* Header dosyası bağlama ve Header Guard (`#ifndef`, `#define`) kullanımı.
* Çoklu dosya linkleme süreci.

### 4. **23 - 30 Mart | Kişisel Not Defteri**

* **Hedef:** Dosya I/O ve Pointer mantığına giriş.
* **Görev:** `notlar.txt` dosyasına veri yazan ve okuyan uygulama.
* **Kazanımlar:**
* Fonksiyonlar: `fopen()`, `fprintf()`, `fscanf()`, `fclose()`
* `FILE *fptr` tanımı ile pointer kullanımı.

### 5. **30 Mart - 06 Nisan | Envanter Yönetim Sistemi**

* **Hedef:** "Call by Reference" (Adres ile çağırma) mantığı.
* **Görev:** Ürün fiyat ve stok adedini pointer kullanarak güncelleyen sistem.
* **Kazanımlar:**
* Operatörler: `*` (Dereferencing) ve `&` (Address-of).
* Pointer aritmetiği ve bellek yönetimi.

### 6. **06 - 13 Nisan | Zıplayan Kare**

* **Hedef:** Raylib çekirdek döngüsünü anlamak.
* **Görev:** Kenarlara çarpınca renk değiştiren, klavye ile kontrol edilen kare.
* **Kazanımlar:**
* Game Loop: `InitWindow()`, `WindowShouldClose()`
* Çizim: `BeginDrawing()`, `ClearBackground()`, `DrawRectangle()`
* Girdi: `IsKeyDown()`

### 7. **13 - 20 Nisan | Statik Izgara Sistemi**

* **Hedef:** Block Blast temeli için 10x10'luk oyun alanı.
* **Görev:** İç içe `for` döngüleri ile ızgara çizimi.
* **Kazanımlar:**
* 2D Array (Çift boyutlu diziler) ile dünya temsili.
* Koordinat dönüşüm matematiği ($x = sütun \times hücre\_boyutu$).

### 8. **20 - 27 Nisan | Sürükle-Bırak Şekiller**

* **Hedef:** Blok taşıma ve "Snapping" (Mıknatıslanma) mekaniği.
* **Görev:** Fare ile tutulan bloğun ızgara hücrelerine tam oturması.
* **Kazanımlar:**
* Çarpışma Kontrolü: `GetMousePosition()`, `CheckCollisionPointRec()`
* Sınır Kontrolü (Boundary check).

### 9. **27 Nisan - 04 Mayıs | Satır Patlatma Mekaniği**

* **Hedef:** Dolu satırları temizleme ve skor sistemi.
* **Görev:** Satır dolduğunda dizideki değerleri sıfırlama.
* **Kazanımlar:**
* Dizi tarama algoritmaları.
* UI ve Ses: `DrawText()`, `LoadSound()`, `PlaySound()`.

### Proje Bağlantıları
| Proje No | Mukaddes | Berivan | Süleyman |
| -------- | -------- | ------- | -------- |
| 1        |          |         |          |
| 2        |          |         |          |
| 3        |          |         |          |
| 4        |          |         |          |
| 5        |          |         |          |
| 6        |          |         |          |
| 7        |          |         |          |
| 8        |          |         |          |
| 9        |          |         |          |
