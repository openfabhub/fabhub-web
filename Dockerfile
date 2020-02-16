# Builder Image
FROM alpine:3.11 AS builder

RUN apk update && apk add --no-cache \
  build-base \
  cmake \
  g++ \
  gcc \
  git \
  libstdc++ \
  linux-headers \
  py-pip && \
  pip --no-cache-dir install conan && \
  conan user

# Build Environment Image
FROM builder AS buildenv

COPY . /src

RUN cmake -S /src \
  -B /build \
  -DCMAKE_BUILD_TYPE=Release && \
  cmake --build /build \
  --target all \
  --parallel $(nproc)

# Deployment Image
FROM scratch

COPY --from=buildenv /build/mofprint /mofprint

CMD ["/mofprint"]