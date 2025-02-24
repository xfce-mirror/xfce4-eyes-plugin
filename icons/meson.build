sizes = [16, 22, 24, 32, 36, 48]

foreach size : sizes
  install_data(
    '@0@x@0@'.format(size) / 'xfce4-eyes.png',
    install_dir: get_option('prefix') / get_option('datadir') / 'icons' / 'hicolor' / '@0@x@0@'.format(size) / 'apps',
  )
endforeach
