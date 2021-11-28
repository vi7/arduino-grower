FROM nginx:1.21.4-alpine

LABEL description="Environment control station web frontend"

COPY ["*.html", "*.htm", "*.js", "*.css", "/usr/share/nginx/html/"]

COPY ["images", "/usr/share/nginx/html/images/"]
