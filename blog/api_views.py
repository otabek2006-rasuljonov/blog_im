from rest_framework import generics, filters
from .models import Post
from .serializers import PostSerializer


class PostListAPIView(generics.ListAPIView):
    queryset = Post.objects.filter(published=True)
    serializer_class = PostSerializer
    filter_backends = [filters.SearchFilter]
    search_fields = ['title', 'content']


class PostDetailAPIView(generics.RetrieveAPIView):
    queryset = Post.objects.filter(published=True)
    serializer_class = PostSerializer
