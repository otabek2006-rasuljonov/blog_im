from django.shortcuts import render, get_object_or_404
from django.core.paginator import Paginator
from django.db.models import Q
from .models import Post, SiteSettings


def post_list(request):
    q = request.GET.get('q', '').strip()
    posts = Post.objects.filter(published=True)
    if q:
        posts = posts.filter(
            Q(title__icontains=q) | Q(content__icontains=q)
        )
    paginator = Paginator(posts, 6)
    page_number = request.GET.get('page')
    page_obj = paginator.get_page(page_number)
    return render(request, 'blog/post_list.html', {
        'page_obj': page_obj,
        'query': q,
    })


def post_detail(request, pk):
    post = get_object_or_404(Post, pk=pk, published=True)
    return render(request, 'blog/post_detail.html', {'post': post})


def about(request):
    settings = SiteSettings.load()
    return render(request, 'blog/about.html', {'about_text': settings.about_text})


def connection(request):
    settings = SiteSettings.load()
    return render(request, 'blog/connection.html', {'settings': settings})
