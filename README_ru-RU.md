# CP Editor

<img src=assets/icon.ico height="80" width="80">

CP Editor - это легкий кросс-платформенный редактор кода на базе Qt, специально разработанный для спортивного программирования.

Этот редактор облегчает жизнь спортивного прораммиста :grin: автоматизируя многие вещи.

[Возможности](https://cpeditor.github.io/) | [Установка](doc/INSTALL_ru-RU.md) | [Использование](doc/MANUAL_ru-RU.md) | [Список изменений](doc/CHANGELOG.md) | [Сотрудничество](CONTRIBUTING_ru-RU.md) | [FAQ](#faq) | [English](README.md) | [中文](README_zh-CN.md)

---

<!-- ALL-CONTRIBUTORS-BADGE:START - Do not remove or modify this section -->
[![All Contributors](https://img.shields.io/badge/all_contributors-36-orange.svg)](#contributors-sparkles)
<!-- ALL-CONTRIBUTORS-BADGE:END -->
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/ce0f297f31f74485b0d340949d08d605)](https://www.codacy.com/gh/cpeditor/cpeditor)
[![Все релизы GitHub](https://img.shields.io/github/downloads/cpeditor/cpeditor/total?label=downloads%40all)](https://github.com/cpeditor/cpeditor/releases)
[![Slack Workspace](https://img.shields.io/badge/join-slack-success)](https://join.slack.com/t/cpeditor/shared_invite/zt-ekfy0zb5-SrOi8SIox8oq61oRonBynw)
[![Группа в Телеграм](https://img.shields.io/badge/join-telegram%20chat-success)](https://t.me/cpeditor)
[![Помощь в отлове багов](https://img.shields.io/github/issues/cpeditor/cpeditor/help%20wanted)](https://github.com/cpeditor/cpeditor/issues?q=is%3Aissue+is%3Aopen+label%3A%22help+wanted%22)

|  type  |                           branch                           |                                                                                                       build                                                                                                        |                                                                   downloads                                                                    |                                                         AUR                                                         |
| :----: | :--------------------------------------------------------: | :----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------: | :--------------------------------------------------------------------------------------------------------------------------------------------: | :-----------------------------------------------------------------------------------------------------------------: |
| alpha  | [master](https://github.com/cpeditor/cpeditor/tree/master) | [![GitHub Workflow Status](https://github.com/cpeditor/cpeditor/workflows/CI:%20Build%20Test/badge.svg?branch=master&event=push)](https://github.com/cpeditor/cpeditor/actions?query=event%3Apush+branch%3Amaster) |                                                                       -                                                                        | [![AUR version](https://img.shields.io/aur/version/cpeditor-git)](https://aur.archlinux.org/packages/cpeditor-git/) |
|  beta  |   [v6.5](https://github.com/cpeditor/cpeditor/tree/v6.5)   |   [![GitHub Workflow Status](https://github.com/cpeditor/cpeditor/workflows/CI:%20Build%20Test/badge.svg?branch=v6.5&event=push)](https://github.com/cpeditor/cpeditor/actions?query=event%3Apush+branch%3Av6.5)   | [![Downloads](https://img.shields.io/github/downloads/cpeditor/cpeditor/6.5.1/total)](https://github.com/cpeditor/cpeditor/releases/tag/6.5.1) |                                                          -                                                          |
| stable |   [v6.4](https://github.com/cpeditor/cpeditor/tree/v6.4)   |   [![GitHub Workflow Status](https://github.com/cpeditor/cpeditor/workflows/CI:%20Build%20Test/badge.svg?branch=v6.4&event=push)](https://github.com/cpeditor/cpeditor/actions?query=event%3Apush+branch%3Av6.4)   | [![Downloads](https://img.shields.io/github/downloads/cpeditor/cpeditor/6.4.6/total)](https://github.com/cpeditor/cpeditor/releases/tag/6.4.6) |     [![AUR version](https://img.shields.io/aur/version/cpeditor)](https://aur.archlinux.org/packages/cpeditor/)     |

## Начало

- [Релизы](https://github.com/cpeditor/cpeditor/releases)
- [Установка разными способами](doc/INSTALL_ru-RU.md)
- [Начало и советы](doc/MANUAL_ru-RU.md)

## Сотрудничество

- [Открыть вопрос](https://github.com/cpeditor/cpeditor/issues/new/choose)
- [Правила внесения вклада](CONTRIBUTING_ru-RU.md)
- [Good first issues](https://github.com/cpeditor/cpeditor/issues?q=is%3Aissue+is%3Aopen+label%3A%22good+first+issue%22)
- [Help wanted issues](https://github.com/cpeditor/cpeditor/issues?q=is%3Aissue+is%3Aopen+label%3A%22help+wanted%22)

## Получить помощь

1. Прочитайте в первую очередь [мануал](doc/MANUAL_ru-RU.md) и [FAQ](#faq), кроме того, просмотрите меню и настройки, чтобы узнать, есть ли то, что вы ищете.
2. Сначала поищите в Google или Яндекс. Может проблема не связана с редактором? Например, "как установить ClangFormat?" или "что такое ошибка компиляции?"
3. Поищите в [Issues](https://github.com/cpeditor/cpeditor/issues) и убедитесь, что такой ошибки/запроса функции еще не было.
4. Пожалуйста, следуйте шаблону вопроса и предоставьте подробную информацию, т.к. это поможет нам дать наиболее качественный ответ на ваш вопрос.
5. Если это запрос функции или сообщение об ошибке, а не вопрос, пожалуйста, создайте тему в [Issues](https://github.com/cpeditor/cpeditor/issues) для более быстрого баг-трекинга, а не спрашивайте в Telegram.

- [Открыть вопрос](https://github.com/cpeditor/cpeditor/issues/new/choose)
- [Спросить в Telegram](https://t.me/cpeditor)
- [Спросить в Slack](https://join.slack.com/t/cpeditor/shared_invite/zt-ekfy0zb5-SrOi8SIox8oq61oRonBynw)

## FAQ

- Я использую Java. Редактор не запускать мой код. Что делать?
   - Вы должны использовать **non-public** класс **а** для своего решения.
- Я получил ошибку **DLL Missing error** во время запуска программы. Что делать?
   - Пожалуйста, установите [Распространяемый компонент Microsoft Visual C++ для Visual Studio 2015, 2017 и 2019](https://support.microsoft.com/ru-ru/help/2977003/the-latest-supported-visual-c-downloads).
- Как включить темную тему во всем приложении?
   - Темная тема доступна только на некоторых платформах, включая KDE и Mac OS. Просто установите системную тему, и CP Editor будет использовать эту же тему. Также можно использовать различные темы с помощью опции командной строки `--style`, но это уже особенность, [которую предоставляет Qt](https://doc.qt.io/qt-5/qstyle.html#details), а не CP Editor. Она работает НЕ НА ВСЕХ платформах.
- Как получить входные/выходные данные с веб-сайтов?
   - Вам нужно установить [Competitive Companion](https://github.com/jmerle/competitive-companion) на Ваш браузер, и использовать его на веб-сайте, пока открыт CP Editor.
- Как послать решение в CF внутри редактора?
   - Вы должны либо отправить задачу через Competitive Companion, либо установить URL-адрес задачи в контекстном меню вкладок. Затем вы увидите кнопку "Submit".
- Я использую его на Codeforces, но кнопка "Submit" неактивна.
   - Это потому, что редактор не может выполнить команду `cf`. Пожалуйста, убедитесь, что он находится в PATH или установите путь в Настройках.
- Когда я нажимаю кнопку "Submit", я получаю какое-то сообщение, что необходим шаблон?
   - Инструмент `cf` требует предварительной настройки. Пожалуйста, запустите `cf config`, чтобы установить имя пользователя и пароль. Затем добавить шаблон.
- Я получил ошибку при копировании входных/выходных данных?
   - Пожалуйста, попробуйте перезагрузить браузер и редактор. Мы знаем, что есть несколько человек, которые получают ошибку даже после перезагрузки (чаще на Google Chrome, чем на Firefox). Нам очень жаль, но если у Вас ещё есть эта ошибка, попробуйте изменить браузер, а пока мы постараемся сделать все возможное, чтобы такой ошибки не было.

## Contributors :sparkles:

Thanks goes to these wonderful people ([emoji key](https://allcontributors.org/docs/en/emoji-key)):

<!-- ALL-CONTRIBUTORS-LIST:START - Do not remove or modify this section -->
<!-- prettier-ignore-start -->
<!-- markdownlint-disable -->
<table>
  <tr>
    <td align="center">
      <a href="https://github.com/ouuan"><img src="https://avatars2.githubusercontent.com/u/30581822?v=4" width="100px;" alt=""/></a><br /><a href="https://github.com/ouuan"><sub><b>Yufan You</b></sub></a><br /><a href="#maintenance-ouuan" title="Maintenance">🚧</a> <a href="https://github.com/cpeditor/cpeditor/commits?author=ouuan" title="Code">💻</a> <a href="https://github.com/cpeditor/cpeditor/commits?author=ouuan" title="Documentation">📖</a> <a href="#translation-ouuan" title="Translation">🌍</a> <a href="#ideas-ouuan" title="Ideas, Planning, & Feedback">🤔</a> <a href="https://github.com/cpeditor/cpeditor/issues?q=author%3Aouuan" title="Bug reports">🐛</a> <a href="https://github.com/cpeditor/cpeditor/pulls?q=is%3Apr+reviewed-by%3Aouuan" title="Reviewed Pull Requests">👀</a>
    </td>
    <td align="center">
      <a href="https://github.com/coder3101"><img src="https://avatars2.githubusercontent.com/u/22212259?v=4" width="100px;" alt=""/></a><br /><a href="https://github.com/coder3101"><sub><b>Ashar</b></sub></a><br /><a href="#maintenance-coder3101" title="Maintenance">🚧</a> <a href="https://github.com/cpeditor/cpeditor/commits?author=coder3101" title="Code">💻</a> <a href="https://github.com/cpeditor/cpeditor/commits?author=coder3101" title="Documentation">📖</a> <a href="#ideas-coder3101" title="Ideas, Planning, & Feedback">🤔</a> <a href="https://github.com/cpeditor/cpeditor/issues?q=author%3Acoder3101" title="Bug reports">🐛</a> <a href="https://github.com/cpeditor/cpeditor/pulls?q=is%3Apr+reviewed-by%3Acoder3101" title="Reviewed Pull Requests">👀</a>
    </td>
    <td align="center">
      <a href="https://github.com/neko-para"><img src="https://avatars1.githubusercontent.com/u/26452623?v=4" width="100px;" alt=""/></a><br /><a href="https://github.com/neko-para"><sub><b>liaohy</b></sub></a><br /><a href="https://github.com/cpeditor/cpeditor/commits?author=neko-para" title="Code">💻</a> <a href="https://github.com/cpeditor/cpeditor/commits?author=neko-para" title="Documentation">📖</a> <a href="#translation-neko-para" title="Translation">🌍</a> <a href="#ideas-neko-para" title="Ideas, Planning, & Feedback">🤔</a> <a href="https://github.com/cpeditor/cpeditor/pulls?q=is%3Apr+reviewed-by%3Aneko-para" title="Reviewed Pull Requests">👀</a>
    </td>
    <td align="center">
      <a href="https://github.com/sadykhzadeh"><img src="https://avatars1.githubusercontent.com/u/51178055?v=4" width="100px;" alt=""/></a><br /><a href="http://sadykhzadeh.github.io"><sub><b>Azer Sadykhzadeh</b></sub></a><br /><a href="#translation-sadykhzadeh" title="Translation">🌍</a>
    </td>
    <td align="center">
      <a href="https://github.com/IZOBRETATEL777"><img src="https://avatars0.githubusercontent.com/u/32099652?v=4" width="100px;" alt=""/></a><br /><a href="http://izobretatelsoft.moy.su/"><sub><b>IZOBRETATEL777</b></sub></a><br /><a href="#translation-IZOBRETATEL777" title="Translation">🌍</a>
    </td>
    <td align="center">
      <a href="https://github.com/Razdeep"><img src="https://avatars2.githubusercontent.com/u/32531173?v=4" width="100px;" alt=""/></a><br /><a href="https://www.linkedin.com/in/razdeeproychowdhury"><sub><b>Rajdeep Roy Chowdhury</b></sub></a><br /><a href="https://github.com/cpeditor/cpeditor/commits?author=Razdeep" title="Code">💻</a> <a href="#ideas-Razdeep" title="Ideas, Planning, & Feedback">🤔</a> <a href="https://github.com/cpeditor/cpeditor/issues?q=author%3ARazdeep" title="Bug reports">🐛</a>
    </td>
  </tr>
  <tr>
    <td align="center">
      <a href="https://github.com/testitem"><img src="https://avatars3.githubusercontent.com/u/19212916?v=4" width="100px;" alt=""/></a><br /><a href="https://github.com/testitem"><sub><b>queen bee shuttle</b></sub></a><br /><a href="https://github.com/cpeditor/cpeditor/commits?author=testitem" title="Documentation">📖</a> <a href="#ideas-testitem" title="Ideas, Planning, & Feedback">🤔</a> <a href="https://github.com/cpeditor/cpeditor/issues?q=author%3Atestitem" title="Bug reports">🐛</a>
    </td>
    <td align="center">
      <a href="https://github.com/yatharth1706"><img src="https://avatars2.githubusercontent.com/u/32243289?v=4" width="100px;" alt=""/></a><br /><a href="https://yatharth1706.github.io/"><sub><b>Yatharth Verma</b></sub></a><br /><a href="https://github.com/cpeditor/cpeditor/commits?author=yatharth1706" title="Code">💻</a> <a href="https://github.com/cpeditor/cpeditor/issues?q=author%3Ayatharth1706" title="Bug reports">🐛</a>
    </td>
    <td align="center">
      <a href="https://github.com/polyomino-24"><img src="https://avatars0.githubusercontent.com/u/20138086?v=4" width="100px;" alt=""/></a><br /><a href="https://github.com/polyomino-24"><sub><b>polyomino</b></sub></a><br /><a href="https://github.com/cpeditor/cpeditor/commits?author=polyomino-24" title="Documentation">📖</a>
    </td>
    <td align="center">
      <a href="https://github.com/StudyingFather"><img src="https://avatars3.githubusercontent.com/u/23295419?v=4" width="100px;" alt=""/></a><br /><a href="https://studyingfather.com"><sub><b>Shuhao Zhang</b></sub></a><br /><a href="#ideas-StudyingFather" title="Ideas, Planning, & Feedback">🤔</a> <a href="https://github.com/cpeditor/cpeditor/pulls?q=is%3Apr+reviewed-by%3AStudyingFather" title="Reviewed Pull Requests">👀</a> <a href="#translation-StudyingFather" title="Translation">🌍</a>
    </td>
    <td align="center">
      <a href="https://github.com/Ravenclaw-OIer"><img src="https://avatars2.githubusercontent.com/u/44526586?v=4" width="100px;" alt=""/></a><br /><a href="http://ravenclaw-oier.github.io"><sub><b>Ravenclaw-OIer</b></sub></a><br /><a href="#ideas-Ravenclaw-OIer" title="Ideas, Planning, & Feedback">🤔</a> <a href="https://github.com/cpeditor/cpeditor/issues?q=author%3ARavenclaw-OIer" title="Bug reports">🐛</a>
    </td>
    <td align="center">
      <a href="https://github.com/tuwuna"><img src="https://avatars3.githubusercontent.com/u/56817415?v=4" width="100px;" alt=""/></a><br /><a href="https://github.com/tuwuna"><sub><b>tuwuna</b></sub></a><br /><a href="#ideas-tuwuna" title="Ideas, Planning, & Feedback">🤔</a>
    </td>
  </tr>
  <tr>
    <td align="center">
      <a href="https://github.com/nikhil-sarda"><img src="https://avatars1.githubusercontent.com/u/51774873?v=4" width="100px;" alt=""/></a><br /><a href="https://github.com/nikhil-sarda"><sub><b>Nikhil Sarda</b></sub></a><br /><a href="https://github.com/cpeditor/cpeditor/issues?q=author%3Anikhil-sarda" title="Bug reports">🐛</a>
    </td>
    <td align="center">
      <a href="https://github.com/Arthur-QQ"><img src="https://avatars0.githubusercontent.com/u/30394349?v=4" width="100px;" alt=""/></a><br /><a href="https://github.com/Arthur-QQ"><sub><b>Arthur-QQ</b></sub></a><br /><a href="#ideas-Arthur-QQ" title="Ideas, Planning, & Feedback">🤔</a> <a href="https://github.com/cpeditor/cpeditor/issues?q=author%3AArthur-QQ" title="Bug reports">🐛</a>
    </td>
    <td align="center">
      <a href="https://github.com/Brijesh1in"><img src="https://avatars3.githubusercontent.com/u/33791137?v=4" width="100px;" alt=""/></a><br /><a href="https://github.com/Brijesh1in"><sub><b>Brijesh1in</b></sub></a><br /><a href="#ideas-Brijesh1in" title="Ideas, Planning, & Feedback">🤔</a> <a href="https://github.com/cpeditor/cpeditor/issues?q=author%3ABrijesh1in" title="Bug reports">🐛</a>
    </td>
    <td align="center">
      <a href="https://github.com/DarkWarrior703"><img src="https://avatars0.githubusercontent.com/u/56077342?v=4" width="100px;" alt=""/></a><br /><a href="https://github.com/DarkWarrior703"><sub><b>DarkWarrior703</b></sub></a><br /><a href="#ideas-DarkWarrior703" title="Ideas, Planning, & Feedback">🤔</a>
    </td>
    <td align="center">
      <a href="https://github.com/1CH1GO"><img src="https://avatars2.githubusercontent.com/u/56096830?v=4" width="100px;" alt=""/></a><br /><a href="http://pavankumar31072000@gmail.com"><sub><b>v-O_O-v</b></sub></a><br /><a href="https://github.com/cpeditor/cpeditor/issues?q=author%3A1CH1GO" title="Bug reports">🐛</a>
    </td>
    <td align="center">
      <a href="https://github.com/ApoorvaRajBhadani"><img src="https://avatars1.githubusercontent.com/u/11313858?v=4" width="100px;" alt=""/></a><br /><a href="http://www.apoorvarajbhadani.blogspot.com"><sub><b>Apoorva Raj Bhadani</b></sub></a><br /><a href="https://github.com/cpeditor/cpeditor/issues?q=author%3AApoorvaRajBhadani" title="Bug reports">🐛</a>
    </td>
  </tr>
  <tr>
    <td align="center">
      <a href="https://github.com/Exile-2k4"><img src="https://avatars2.githubusercontent.com/u/40086604?v=4" width="100px;" alt=""/></a><br /><a href="https://github.com/Exile-2k4"><sub><b>Nguyễn Hoàng Dương</b></sub></a><br /><a href="#ideas-Exile-2k4" title="Ideas, Planning, & Feedback">🤔</a>
    </td>
    <td align="center">
      <a href="https://github.com/GoBigorGoHome"><img src="https://avatars3.githubusercontent.com/u/9896288?v=4" width="100px;" alt=""/></a><br /><a href="https://github.com/GoBigorGoHome"><sub><b>Pat</b></sub></a><br /><a href="#ideas-GoBigorGoHome" title="Ideas, Planning, & Feedback">🤔</a>
    </td>
    <td align="center">
      <a href="https://github.com/GoToCoding"><img src="https://avatars3.githubusercontent.com/u/16542586?v=4" width="100px;" alt=""/></a><br /><a href="https://github.com/GoToCoding"><sub><b>Ruzal Ibragimov</b></sub></a><br /><a href="https://github.com/cpeditor/cpeditor/issues?q=author%3AGoToCoding" title="Bug reports">🐛</a>
    </td>
    <td align="center">
      <a href="https://github.com/Salil03"><img src="https://avatars0.githubusercontent.com/u/32109637?v=4" width="100px;" alt=""/></a><br /><a href="https://salil03.github.io/"><sub><b>Salil Gokhale</b></sub></a><br /><a href="#ideas-Salil03" title="Ideas, Planning, & Feedback">🤔</a>
    </td>
    <td align="center">
      <a href="https://github.com/WYWFlash"><img src="https://avatars1.githubusercontent.com/u/42882043?v=4" width="100px;" alt=""/></a><br /><a href="https://github.com/WYWFlash"><sub><b>WYWFlash</b></sub></a><br /><a href="https://github.com/cpeditor/cpeditor/issues?q=author%3AWYWFlash" title="Bug reports">🐛</a>
    </td>
    <td align="center">
      <a href="https://github.com/WZKSDN"><img src="https://avatars1.githubusercontent.com/u/14256391?v=4" width="100px;" alt=""/></a><br /><a href="https://github.com/WZKSDN"><sub><b>nocriz</b></sub></a><br /><a href="https://github.com/cpeditor/cpeditor/issues?q=author%3AWZKSDN" title="Bug reports">🐛</a>
    </td>
  </tr>
  <tr>
    <td align="center">
      <a href="https://github.com/Wandoka"><img src="https://avatars2.githubusercontent.com/u/26433142?v=4" width="100px;" alt=""/></a><br /><a href="https://github.com/Wandoka"><sub><b>Wandoka</b></sub></a><br /><a href="#ideas-Wandoka" title="Ideas, Planning, & Feedback">🤔</a>
    </td>
    <td align="center">
      <a href="https://github.com/ahmfuad"><img src="https://avatars3.githubusercontent.com/u/42442668?v=4" width="100px;" alt=""/></a><br /><a href="http://www.agamiramra.com"><sub><b>A. H. M Fuad</b></sub></a><br /><a href="https://github.com/cpeditor/cpeditor/issues?q=author%3Aahmfuad" title="Bug reports">🐛</a>
    </td>
    <td align="center">
      <a href="https://github.com/ankur314"><img src="https://avatars2.githubusercontent.com/u/37039680?v=4" width="100px;" alt=""/></a><br /><a href="https://github.com/ankur314"><sub><b>Ankur Ojha</b></sub></a><br /><a href="#ideas-ankur314" title="Ideas, Planning, & Feedback">🤔</a>
    </td>
    <td align="center">
      <a href="https://github.com/cs-mshah"><img src="https://avatars1.githubusercontent.com/u/56499208?v=4" width="100px;" alt=""/></a><br /><a href="https://github.com/cs-mshah"><sub><b>cs-mshah</b></sub></a><br /><a href="#ideas-cs-mshah" title="Ideas, Planning, & Feedback">🤔</a>
    </td>
    <td align="center">
      <a href="https://github.com/fedimser"><img src="https://avatars3.githubusercontent.com/u/14271071?v=4" width="100px;" alt=""/></a><br /><a href="http://fedimser.github.io"><sub><b>Dmytro Fedoriaka</b></sub></a><br /><a href="https://github.com/cpeditor/cpeditor/issues?q=author%3Afedimser" title="Bug reports">🐛</a>
    </td>
    <td align="center">
      <a href="https://github.com/m3h3d1"><img src="https://avatars0.githubusercontent.com/u/39239695?v=4" width="100px;" alt=""/></a><br /><a href="https://github.com/m3h3d1"><sub><b>m3h3d1</b></sub></a><br /><a href="#ideas-m3h3d1" title="Ideas, Planning, & Feedback">🤔</a>
    </td>
  </tr>
  <tr>
    <td align="center">
      <a href="https://github.com/rng70"><img src="https://avatars2.githubusercontent.com/u/45529910?v=4" width="100px;" alt=""/></a><br /><a href="https://github.com/rng70"><sub><b>Al Arafat Tanin</b></sub></a><br /><a href="#ideas-rng70" title="Ideas, Planning, & Feedback">🤔</a>
    </td>
    <td align="center">
      <a href="https://github.com/s6059r"><img src="https://avatars0.githubusercontent.com/u/58967102?v=4" width="100px;" alt=""/></a><br /><a href="https://github.com/s6059r"><sub><b>Siddharth Singh</b></sub></a><br /><a href="#ideas-s6059r" title="Ideas, Planning, & Feedback">🤔</a>
    </td>
    <td align="center">
      <a href="https://github.com/samael042"><img src="https://avatars3.githubusercontent.com/u/44134245?v=4" width="100px;" alt=""/></a><br /><a href="https://github.com/samael042"><sub><b>samael042</b></sub></a><br /><a href="https://github.com/cpeditor/cpeditor/issues?q=author%3Asamael042" title="Bug reports">🐛</a>
    </td>
    <td align="center">
      <a href="https://github.com/2740365712"><img src="https://avatars0.githubusercontent.com/u/33799658?v=4" width="100px;" alt=""/></a><br /><a href="https://github.com/2740365712"><sub><b>Starlit_Night</b></sub></a><br /><a href="#ideas-2740365712" title="Ideas, Planning, & Feedback">🤔</a>
    </td>
    <td align="center">
      <a href="https://github.com/shakeabi"><img src="https://avatars2.githubusercontent.com/u/36559835?v=4" width="100px;" alt=""/></a><br /><a href="http://abishake.co"><sub><b>Abishake</b></sub></a><br /><a href="#ideas-shakeabi" title="Ideas, Planning, & Feedback">🤔</a>
    </td>
    <td align="center">
      <a href="https://github.com/sahil-ra"><img src="https://avatars0.githubusercontent.com/u/30044253?v=4" width="100px;" alt=""/></a><br /><a href="https://github.com/sahil-ra"><sub><b>sahil-ra</b></sub></a><br /><a href="#ideas-sahil-ra" title="Ideas, Planning, & Feedback">🤔</a>
    </td>
  </tr>
</table>

<!-- markdownlint-enable -->
<!-- prettier-ignore-end -->
<!-- ALL-CONTRIBUTORS-LIST:END -->

This project follows the [all-contributors](https://github.com/all-contributors/all-contributors) specification. Contributions of any kind welcome!

Лицензия
----

[GNU General Public License Version 3.0](https://github.com/cpeditor/cpeditor/blob/master/LICENSE)
