# Codebase Guide

Bu dokuman, projeyi ilk kez acan birinin "nereden baslamaliyim?" sorusuna cevap vermek icin hazirlandi. Amac her fonksiyonu anlatmak degil; oyunun buyuk resmini, ana akislari ve dosyalarin sorumluluklarini gostermek.

## 1. Buyuk Resim

Oyun raylib'in klasik oyun dongusuyle calisir: once pencere ve kaynaklar hazirlanir, sonra her frame'de input, update ve render adimlari calisir.

```mermaid
flowchart TD
    main["main.c\nPencere, font, texture, sound, ana dongu"]
    game["game.c\nEkran akisi, oyun reset, menu/settings/result"]
    input["input.c\nSurukle-birak ve hamle sonucu"]
    board["board.c\n8x8 tahta kurallari"]
    piece["piece.c\nParca sekilleri ve slotlar"]
    level["level.c\nLevel hedefleri ve levels.txt"]
    save["save.c\nplayer/scoreboard/progress txt kayitlari"]
    render["render.c\nOyun alani, tahta, parcalar"]
    renderui["render_ui.c\nMenu, HUD, settings, result, scoreboard"]
    effects["effects.c\nAnimasyon, particle, floating text"]
    sound["sound.c\nSes efektleri ve muzik"]
    textures["textures.c\nGorsel dosyalari yukleme"]

    main --> game
    main --> input
    main --> render
    main --> sound
    main --> textures

    game --> level
    game --> save
    game --> sound
    game --> effects

    input --> board
    input --> piece
    input --> level
    input --> effects
    input --> sound

    render --> board
    render --> effects
    render --> renderui
    renderui --> level
    renderui --> textures
```

## 2. Ana Oyun Dongusu

`main.c` cok kucuk tutulur. Asil fikir sudur: oyun durumunu guncelle, sonra ekrana ciz.

```mermaid
sequenceDiagram
    participant Main as main.c
    participant Input as input.c
    participant Game as game.c
    participant Render as render.c / render_ui.c

    Main->>Game: GameInit(&state)
    loop Her frame
        Main->>Input: InputUpdate(&state) [sadece PLAY ekraninda]
        Main->>Game: GameUpdate(&state)
        Main->>Render: RenderFrame(&state)
    end
```

Okurken once su sirayi takip etmek iyi olur:

1. `src/main.c`: Program nerede basliyor?
2. `include/game.h`: `GameState` icinde hangi bilgiler tutuluyor?
3. `src/game.c`: Hangi ekrandayiz, butona basilinca nereye gidiyoruz?
4. `src/input.c`: Bir parca tahtaya birakilinca ne oluyor?
5. `src/board.c` ve `src/piece.c`: Tahta ve parca kurallari nasil calisiyor?

## 3. Hamle Akisi

Oyuncu bir parcayi surukleyip tahtaya biraktiginda birden fazla sistem devreye girer. Bu akis oyunun cekirdegidir.

```mermaid
flowchart TD
    start["Mouse ile parca secilir"]
    drag["Parca suruklenir"]
    drop["Mouse birakilir"]
    canPlace{"BoardCanPlace?"}
    place["BoardPlace\nParca tahtaya yazilir"]
    clear["BoardClearLines\nDolu satir/sutun temizlenir"]
    score["Puan, combo ve level hedefleri guncellenir"]
    effects["Animasyon, particle, floating text eklenir"]
    slot["SlotClear\nKullanilan parca silinir"]
    next{"3 slot da bos mu?"}
    newPieces["GenerateRandomPieces\nYeni 3 parca gelir"]
    invalid["Hamle gecersiz\nParca eski yerine donmus gibi olur"]

    start --> drag --> drop --> canPlace
    canPlace -- hayir --> invalid
    canPlace -- evet --> place --> clear --> score --> effects --> slot --> next
    next -- evet --> newPieces
    next -- hayir --> done["Frame devam eder"]
```

Bu akisin ana dosyasi `src/input.c` dosyasidir. Tahta kurallari `src/board.c`, parca uretimi `src/piece.c`, gorsel geri bildirimler `src/effects.c` icindedir.

## 4. Ekran Akisi

Oyunda birden fazla ekran var ama hepsi tek enum ile takip edilir: `Screen currentScreen`.

```mermaid
stateDiagram-v2
    [*] --> MENU
    MENU --> NICKNAME: Classic Mode ve nickname yok
    NICKNAME --> PLAY: Isim girildi
    MENU --> PLAY: Classic Mode
    MENU --> LEVEL_SELECT: Adventure Mode
    MENU --> MENU_SETTINGS: Gear
    MENU --> SCOREBOARD: Scoreboard

    LEVEL_SELECT --> PLAY: Acik level secildi
    LEVEL_SELECT --> MENU: Back / ESC

    PLAY --> SETTINGS: Gear
    SETTINGS --> PLAY: ESC
    SETTINGS --> MENU: Home
    SETTINGS --> PLAY: Replay

    PLAY --> RESULT: Level bitti veya hamle kalmadi
    RESULT --> PLAY: Retry / Next Level
    RESULT --> MENU: Classic Home
    RESULT --> LEVEL_SELECT: Adventure Home

    MENU_SETTINGS --> NICKNAME: Change Nickname
    MENU_SETTINGS --> MENU: Home / ESC
    SCOREBOARD --> MENU: Back / ESC
```

Bu akisin ana dosyasi `src/game.c` dosyasidir. Cizim kismi `src/render_ui.c` icindedir.

## 5. Veri Dosyalari

Oyun icindeki duzenlenebilir ve kaydedilebilir bilgiler `data/` klasorunde tutulur. Bu klasor `.gitignore` icindedir.

```mermaid
flowchart LR
    levels["data/levels.txt\nLevel hedefleri"]
    progress["data/progress.txt\nunlockedLevel"]
    player["data/player.txt\nnickname + highScore"]
    scoreboard["data/scoreboard.txt\nscore|name"]

    levelc["level.c"] --> levels
    savec["save.c"] --> progress
    savec --> player
    savec --> scoreboard
    gamec["game.c"] --> savec
```

En onemli iki dosya:

- `data/levels.txt`: Hocalar level hedeflerini buradan degistirebilir.
- `data/progress.txt`: Oyuncunun en son hangi level'a kadar actigini tutar.

`progress.txt` ornegi:

```text
unlockedLevel=8
```

Bu, 1-7 arasi level'lar gecildi ve 8. level acik demektir.

## 6. Dosya Sorumluluklari

| Dosya | Ne is yapar? | Ne zaman okunur? |
|---|---|---|
| `main.c` | Programi baslatir, ana donguyu calistirir | Ilk okunacak dosya |
| `game.c` | Ekran gecisleri, oyun reset, sonuc ve settings mantigi | "Oyun hangi ekranda?" sorusu icin |
| `input.c` | Parca surukle-birak ve hamle sonucu | "Hamle nasil isleniyor?" sorusu icin |
| `board.c` | Tahtaya parca sigar mi, satir/sutun temizlenir mi? | Oyun kurallari icin |
| `piece.c` | Parca sekilleri, renkleri ve slotlar | Parcalar nasil uretiliyor? |
| `level.c` | Level hedefleri ve hedef kontrolu | Macera modu icin |
| `save.c` | Metin kayit dosyalari | Kayitlar nerede? |
| `render.c` | Oyun tahtasi ve parcalari cizer | PLAY ekraninin cizimi icin |
| `render_ui.c` | Menu, ayarlar, scoreboard, result cizer | UI ekranlari icin |
| `effects.c` | Animasyon, particle, floating text | Gorsel susler icin |
| `sound.c` | Ses ve muzik | Ses davranisi icin |
| `textures.c` | Image dosyalarini yukler | Asset yukleme icin |

## 7. Sunumda Anlatma Sirasi

Arkadaslara veya hocaya anlatirken su 5 dakikalik rota ise yarar:

1. `main.c`: "Raylib penceresi aciliyor, sonra her frame update + render calisiyor."
2. `GameState`: "Oyundaki tum anlik bilgiler burada."
3. `input.c`: "Oyuncu parcayi birakinca tahta kontrol ediliyor."
4. `board.c`: "Satir/sutun doluysa temizleniyor."
5. `save.c` ve `data/levels.txt`: "Level ve kullanici verileri metin dosyalarindan geliyor."

Bu rota once ana fikri verir, sonra detaylara inmeyi kolaylastirir.
