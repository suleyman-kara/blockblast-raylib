# Block Blast - C ve Raylib

Bu proje, C ve raylib ile yazılmış basit bir Block Blast klonudur. Hedefi çok büyük bir oyun yapmak değil; tahta mantığı, parça üretimi, sürükle-bırak, puanlama, level hedefleri, kayıt dosyaları ve temel görsel/işitsel geri bildirimleri anlaşılır şekilde göstermektir.

## Derleme ve Çalıştırma

Gerekenler:

- Python 3
- C derleyicisi (`gcc`, `clang` veya MinGW)
- İsteğe bağlı olarak `make`

Komutlar:

```bash
python3 build.py              # Derle ve çalıştır
python3 build.py --no-run     # Sadece derle
python3 build.py --debug      # Debug build
python3 build.py --clean      # build/ klasörünü temizle
python3 build.py --no-make    # raylib Makefile kullanmadan derle
```

## Proje Yapısı

```text
src/main.c            Program girişi, pencere ve ana döngü
src/game.c            Oyunu başlatma, resetleme, ekran akışı ve ayarlar
src/save.c            Player, scoreboard ve progress metin kayıtları
src/input.c           Sürükle-bırak ve hamle sonrası puan/level güncelleme
src/board.c           8x8 tahta, yerleştirme ve satır/sütun temizleme
src/piece.c           Parça şekilleri, rastgele parça üretimi ve slotlar
src/level.c           Level hedefleri ve levels.txt okuma
src/render.c          Oyun ekranı, tahta, parçalar ve frame çizimi
src/render_ui.c       Menü, HUD, ayarlar, sonuç ve scoreboard çizimi
src/effects.c         Animasyon, parçacık ve yüzen yazı efektleri
src/sound.c           Ses ve müzik
include/*.h           Ortak tipler ve fonksiyon bildirimleri
assets/               Font, görsel ve ses dosyaları
raylib/               Projeyle gelen raylib kaynakları
```

## Düzenlenebilir Dosyalar

Oyun açılırken `data/` klasörü oluşturulur. Bu klasör `.gitignore` içindedir; yani kişisel kayıtlar repoya eklenmez.

### Level Ayarları

İlk çalıştırmada `data/levels.txt` yoksa oyun varsayılan dosyayı oluşturur. Hocalar veya geliştiriciler level hedeflerini bu dosyadan hızlıca değiştirebilir.

Format:

```text
# level targetScore targetDiamonds targetEmeralds prefillCount
# 0 means this target is disabled.
1 500 0 0 0
2 1000 0 0 0
3 0 3 0 2
```

Alanlar:

- `level`: 1-10 arası macera level numarası
- `targetScore`: istenen skor, `0` ise skor hedefi yok
- `targetDiamonds`: toplanacak elmas sayısı, `0` ise hedef yok
- `targetEmeralds`: toplanacak zümrüt sayısı, `0` ise hedef yok
- `prefillCount`: level başında tahtaya rastgele kaç parça yerleşsin

Boş satırlar, `#` ile başlayan yorumlar ve hatalı satırlar yok sayılır. Geçersiz satır varsa oyun varsayılan değeri kullanmaya devam eder.

### Kayıt Dosyaları

Kayıtlar binary değil, okunabilir metin dosyalarıdır:

```text
data/player.txt      nickname ve highScore
data/scoreboard.txt  score|name formatında skor tablosu
data/progress.txt    level completed formatında macera ilerlemesi
```

Örnek `player.txt`:

```text
nickname=Ali
highScore=1200
```

Örnek `scoreboard.txt`:

```text
1200|Ali
900|Zeynep
```

Örnek `progress.txt`:

```text
1 1
2 0
3 0
```

## Temel Oynanış

1. Ana menüden Classic Mode veya Adventure Mode seçilir.
2. Alt paneldeki 3 parçadan biri sürüklenip 8x8 tahtaya bırakılır.
3. Dolu satır veya sütun oluşursa temizlenir ve puan kazanılır.
4. Art arda temizleme yapılırsa combo artar.
5. Hiçbir parça tahtaya sığmazsa oyun biter.
6. Adventure modunda skor, elmas veya zümrüt hedefleri tamamlanınca level biter.

## Kodun Öğrenme Notları

- `PieceSlot` artık dinamik bellek kullanmaz; slot içinde doğrudan `Piece` ve `occupied` bilgisi tutulur.
- Parçalar az sayıda ve sabit olduğu için `malloc/free` yerine düz struct kullanmak daha okunur ve güvenlidir.
- Settings ekranındaki ortak koordinatlar `include/ui_layout.h` içindeki helper ile hesaplanır.
- Oyun mantığı ve çizim kodu ayrı dosyalardadır; önce `main.c`, sonra `game.c`, `input.c`, `board.c` ve `piece.c` okunması önerilir.
