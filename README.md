# Blog IM — Zamonaviy Minimal Blog

Django, Django REST Framework va CKEditor bilan yaratilgan professional blog sayti.

## O'rnatish

1. Repositoriyani klonlang:
```
git clone https://github.com/otabek2006-rasuljonov/blog_im.git
cd blog_im
```

2. Virtual muhit yarating:
```
python -m venv venv
source venv/bin/activate  # Windows: venv\Scripts\activate
```

3. Paketlarni o'rnating:
```
pip install -r requirements.txt
```

4. Ma'lumotlar bazasini tayyorlang:
```
python manage.py migrate
```

5. Admin foydalanuvchi yarating:
```
python manage.py createsuperuser
```

6. Serverni ishga tushiring:
```
python manage.py runserver
```

7. Brauzerda oching:
- Sayt: http://127.0.0.1:8000/
- Admin: http://127.0.0.1:8000/admin/

## Bo'limlar
- **Posts** — Barcha postlar (asosiy sahifa)
- **About** — Blog haqida
- **Connection** — Bog'lanish (GitHub, Telegram, Telefon, Email)

## API
- GET /api/posts/ — Barcha postlar
- GET /api/posts/?search=kalit_so'z — Qidiruv
- GET /api/posts/<id>/ — Bitta post
