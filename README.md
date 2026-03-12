# Blog IM

Zamonaviy minimal blog sayti — Django + DRF + CKEditor

---

## 🌐 IP-in-IP Tarmoq Monitoringi va Filtratsiyasi (C++)

Bu repozitoriyada C++ yordamida IP-in-IP (RFC 2003) asosida tarmoq
monitoringi va filtratsiyasi tizimi mavjud.

### Tezkor Boshlash

```bash
cd cpp
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Terminal 1 — Filtrator (10.0.0.2 da, root bilan)
sudo ./build/filtrator

# Terminal 2 — Sniffer (10.0.0.1 da, root bilan)
sudo ./build/sniffer
```

Batafsil o'rganish rejasi va resurslar: **[LEARNING_PLAN.md](LEARNING_PLAN.md)**