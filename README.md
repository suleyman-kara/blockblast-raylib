# Block Blast — C ve Raylib ile Geliştirilmiş Blok Oyunu

## Proje Hakkında
Bu proje, popüler Block Blast oyununun C programlama dili ve Raylib kütüphanesi kullanılarak geliştirilmiş bir klonudur. Düşük seviye programlama becerileri (bellek yönetimi, pointer işlemleri) ile oyun geliştirme dinamiklerini birleştiren modüler bir yapı hedeflenmiştir.

---

## Kurulum ve Derleme

Proje, **Python 3** tabanlı bir build script'i (`build.py`) ile derlenir. Script, Windows, Linux ve macOS'te sorunsuz çalışacak şekilde tasarlanmıştır.

### Gereksinimler

| Araç | Açıklama |
|------|----------|
| **Python 3** | Build script'ini çalıştırmak için |
| **C Derleyicisi** (gcc/clang) | Kaynak kodları derlemek için |
| **make** (opsiyonel) | Varsa raylib Makefile'ı kullanılır, yoksa doğrudan derleme yapılır |

**İşletim sistemine göre kurulum:**

- **Linux:** `sudo apt install build-essential python3`
- **macOS:** `xcode-select --install` (gcc/clang dahil)
- **Windows:** [MinGW-w64](https://www.mingw-w64.org/) veya WSL kurulumu

### Derleme ve Çalıştırma

```bash
# Derle ve otomatik çalıştır (varsayılan)
python build.py

# Sadece derle, çalıştırma
python build.py --no-run

# Debug modunda derle
python build.py --debug

# Build klasörünü temizle
python build.py --clean

# make olmadan doğrudan derle
python build.py --no-make
```

Build başarılı olduğunda terminalde `file://build/blockblast` şeklinde tıklanabilir bir yol gösterilir.

### VS Code ile Derleme
1. Python 3 ve bir C derleyicisi kurulu olduğundan emin olun
2. Projeyi VS Code ile açın
3. Terminal'de `python build.py` komutunu çalıştırın
4. Oyun otomatik olarak başlayacaktır

---

## Proje Yapısı

```
├── build.py              # Python build script'i
├── src/                  # Kaynak kodları
│   ├── main.c            # Giriş noktası
│   ├── game.c/h          # Oyun durumu ve ekran yönetimi
│   ├── board.c/h         # 8x8 tahta mantığı
│   ├── piece.c/h         # Blok parçaları ve rastgele üretim
│   ├── input.c/h         # Sürükle-bırak giriş işlemleri
│   ├── render.c/h        # Görsel çizim (tahta, parçalar, menü)
│   ├── anim.c/h          # Satır/sütun temizleme animasyonu
│   ├── particle.c/h      # Parçacık efekti sistemi
│   ├── float_text.c/h    # Yüzen puan metinleri
│   ├── score.c/h         # Puan hesaplama ve highscore
│   └── sound.c/h         # Ses sistemi
├── include/              # Header dosyaları
├── assets/sounds/        # Ses efektleri ve müzik
├── raylib/               # Raylib kütüphanesi (v6.0)
└── build/                # Derleme çıktıları
```

---

## Teknik Detaylar

### Veri Yapıları
- **Board:** 8×8 tam sayı matrisi (`int cells[8][8]`)
- **Piece:** Şekil matrisi (`int shape[5][5]`) + renk index + boyut
- **PieceSlot:** Parça + ekran konumu (sürükleme paneli için)

### Bellek Yönetimi
- Parçalar dinamik olarak `malloc` ile oluşturulur, `free` ile temizlenir
- Animasyon/partikül kuyrukları statik dizi olarak tutulur

### Algoritmalar
- **Satır/Sütun Tarama:** Dolu satır/sütunları tespit edip temizler
- **Puan Hesaplama:** `Base × N × Combo` (N: aynı anda temizlenen çizgi sayısı)
- **Game Over Kontrolü:** Kalan parçaların tahtaya sığıp sığmadığı simüle edilir

### Animasyon Sistemi
- **Clear Animasyonu:** Temizlenen hücreler 0.35sn'de solarak kaybolur
- **Parçacık Sistemi:** Her temizlemede 5 parçacık + varyasyon
- **Float Text:** "+300" gibi puan metinleri yukarı doğru uçar ve kaybolur
- **Combo Banner:** 3+ combo'da "BEST COMBO! x3" banner'ı gösterilir

### Dosya Okuma/Yazma
- Highscore `data/highscore.dat` dosyasına kaydedilir
- `MakeDirectory("data")` ile otomatik oluşturulur

### Ses Sistemi
- 7 ses efekti (yerleştirme, temizleme, combo, menü...)
- Arka plan müziği (WAV, loop)
- Ses/Müzik aç/kapa ayarlar menüsünden yapılır

---

## Oynanış

1. **Ana Menü:** Standart Mod / Macera Modu (çok yakında)
2. **Oyun:** Alt paneldeki 3 parçayı sürükleyerek 8×8 tahtaya yerleştirin
3. **Puanlama:** Tam satır veya sütun oluşturun → temizlenir → puan kazanırsınız
4. **Combo:** Ardışık temizlemeler combo çarpanını artırır
5. **Game Over:** Hiçbir parça sığmazsa oyun biter
6. **Ayarlar:** Sağ üstteki dişli ikonundan ses/müzik ayarları, yeniden başlatma

---

## Teslimat ve Değerlendirme

Teslimat: Kaynak kod, proje raporu, maksimum 5 dakikalık demo videosu ve canlı sunum.

| Kriter | Ağırlık |
|--------|---------|
| Zorunlu Programlama Kavramlarının Kullanımı | 25% |
| Sözlü Sunum ve Jüri Performansı | 25% |
| Kod Kalitesi ve Doğruluk | 20% |
| Yaratıcılık ve Oyun Tasarımı | 15% |
| Rapor ve Dokümantasyon | 10% |
| Video Sunumu | 5% |

### Yapılacaklar
- [ ] Macera modu eklenecek
- [ ] Toplanan puana göre arka plan renk değişimi
- [ ] Görsel ve oynanışa bağlı iyileştirmeler
- [ ] Birlikte patlayanlar kombo sayılmıyor, düzeltilmeli