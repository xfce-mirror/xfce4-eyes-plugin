themes = [
  'Bizarre',
  'Bloodshot',
  'Chibi',
  'Default',
  'Default-tiny',
  'Horrid',
  'Tango',
]

foreach theme : themes
  theme_files = [
    '@0@-eye.png'.format(theme),
    '@0@-pupil.png'.format(theme),
    'config',
  ]

  foreach theme_file : theme_files
    install_data(
      theme / theme_file,
      install_dir: get_option('prefix') / get_option('datadir') / 'xfce4' / 'eyes' / 'themes' / theme,
    )
  endforeach
endforeach
