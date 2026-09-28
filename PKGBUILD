# Maintainer: GFaster Team <starlord1270>
pkgname=gfaster
_pkgname=baloo
pkgver=6.30.0
pkgrel=1
pkgdesc="GFaster - Indexador de archivos ultra-optimizado (fork de Baloo) con menor consumo de CPU/Disco, motor en Rust y compactación LMDB"
arch=('x86_64' 'aarch64')
url="https://invent.kde.org/frameworks/baloo"
license=('LGPL-2.0-only' 'LGPL-3.0-only')
depends=(
  'glibc'
  'kconfig'
  'kcoreaddons'
  'kcrash'
  'kdbusaddons'
  'kfilemetadata'
  'ki18n'
  'kidletime'
  'kio'
  'libstdc++'
  'lmdb'
  'qt6-base'
  'solid'
)
optdepends=(
  'qt6-declarative: QML bindings'
)
makedepends=(
  'extra-cmake-modules>=6.30.0'
  'cmake'
  'ninja'
)
provides=("baloo=${pkgver}" "kf6-baloo=${pkgver}")
conflicts=('baloo' 'baloo5')
replaces=('baloo5')
source=()

build() {
  SRC_DIR="${startdir:-$srcdir}"
  
  cmake -B build -S "$SRC_DIR" \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr \
    -DBUILD_TESTING=OFF \
    -DKDE_INSTALL_LIBEXECDIR=lib \
    -DKDE_INSTALL_SYSCONFDIR=/etc \
    -DCMAKE_CXX_FLAGS="${CXXFLAGS} -O3"
  cmake --build build
}

package() {
  DESTDIR="$pkgdir" cmake --install build
}
