﻿# Extract and import maps of CS:GO

## English version

Basic steps are in [Import CS:GO Maps](https://www.youtube.com/watch?v=pX2ddaJzFHw&ab_channel=ItsJustChris)

### 1. Preparation

- Extract a map using tool [bspsrc](https://github.com/ata4/bspsrc/releases)
- Delete all stuff from scene Blender created at start
- Open Blender Windows Console (not necessary but importing process can be monitored)
    * `Window->Toggle System Console`

### 2. Import

- Open Blender and go to `File->Import->Valve Map Format (.vmf)`
- Select an extracted map (e.g., `de_mirage_d.vmf`)
- In the right bar (configuration) change the following parameters:
    * Sets `Sky resolution` to 1024
    * Sets `Scale` to 0.01 or something similar, but it's better to choose multiples of 10
      (I've chosen 0.02 and ended with speed should be multiplied by 2)
    * Save these parameters as preset (simpler in future) to import another map
- Wait until the selected map is imported

### 3. Blender postprocess

- Select shading mode viewport in the right upper corner (it's necessary to load texture of the map)
- Select "SkyBoxes": elements of the map that are located away from it
- Goto `Object->Transform VMF 3D Sky` (Object placed under Help/Pipeline menus)
- Also remove warmup scenes (if needed)
- Save the result as Blender file in some directory (e.g., `de_mirage.blend`)
- After that (!!!) extract texture from the map:
    * Select all objects on scene
    * Goto `File->External Data->Unpack Resources`
    * Select `Write files to current directory` (this won't work if *.blend wasn't saved before)
    * *Also note that some texture may be not correctly extracted and `VTFEdit` required to get it

- `Save as` *.blend file and in save menu open preference pane (gear in the right upper corner) check `Compress`.
  **This will remove all textures from blend file (textures now stored in the directory near blend file)
  and compress it, that significantly decrease size**

- Export the map as FBX into the same directory where *.blend have been saved (all configurations are defaults).
  **Don't change `Scale` here (!!!) because it was taken into account during VMF import**
- In the import menu, change the next configurations:
    * `Hierarchy Type` change to `Create Level Actors`
    * Check `Create Content Folder Hierarchy`
    * Remove skeletal meshes (if its not needed)

### 4. Import into UE4 and postprocess

Scripts must be executed in order below:

- Create level in UE4 and import this FBX into it using `File->Import Into Level`
- Fix static collisions with [FixStaticActorsCollisions.py](../../Content/Python/Map/FixStaticActorsCollisions.py)
    * By default, all collisions generated by UE4 during import are f**ked up
      (looks like due to the other CS:GO vertex traversal sequence). Script convert all collisions to complex
      and remove geometry with completely unsupported collisions.
- Fix dynamic props collisions
  with [FixPhysicsPropsCollisions.py](../../Content/Python/Map/FixPhysicsPropsCollisions.py)
- Fix environment lightning on the map with [FixEnvironmentLight.py](../../Content/Python/Map/FixEnvironmentLight.py)
    * Imported `light_environment` lightning should be replaced because it is static thus no shadows etc
- Fix light's sources of the map with [FixLightSpotsSources.py](../../Content/Python/Map/FixLightSpotsSources.py)
    * All lights are `Static` other than `Stationary`, so if shadows and good lightning
      needed then all `light_*` actors required to be fixed
    * Imported lights are usually too bright about 10 times than normal
- Fix opacity of materials with textures with Alpha-layer
  with [FixDecalsOverlays.py](../../Content/Python/Map/FixDecalsOverlays.py)
    * `Blend mode` should be set to `Translucent` in material node
    * Alpha channel of texture should be connected to `Opaque pin`
    * See also https://docs.unrealengine.com/5.0/en-US/using-transparency-in-unreal-engine-materials/
- Removed failed TriMesh with [FixFailedTriMesh.py](../../Content/Python/Map/FixFailedTriMesh.py)
    * Some meshes are invalid, and UE4 can't process it correctly (UE4 will print it in log file)
    * Store log file with 'Failed to cook TriMesh' errors
      as `<map_name>.txt` in [fixups directory](../../Importing/fixups)

P.S.: Scripts made separated to each other and each time we
reiterate over all actors to make possible to run it independently.

### 5. Catalog imported map stuff in UE4

## Russian version

Основные шаги доступны в Импорт CS:GO карт

### 1. Подготовка

- Извлечь карту с помощью инструмента [bspsrc](https://github.com/ata4/bspsrc/releases)
- Удалить все объекты, созданные в Blender при старте
- Открыть консольное окно Blender (необязательно, но процесс импорта можно отслеживать)
    * `Окно->Toggle System Console`

### 2. Импорт

- Открыть Blender и перейти `Файл->Импорт->Формат Valve Map (.vmf)`
- Выбрать извлеченную карту (например, `de_mirage_d.vmf`)
- В правой панели (конфигурация) изменить следующие параметры:
    - Установить `Sky resolution` на 1024
    - Установить `Scale` на 0.01 или что-то подобное, но лучше выбирать кратные 10 (Я выбрал 0.02 и получилось, что
      скорость должна быть умножена на 2 ... да, я гений)
    - Сохранить эти параметры как пресет (будет проще в будущем импортировать другую карту)
- Дождитесь завершения импорта выбранной карты

### 3. Постобработка в Blender

- Выбрать режим отображения `shading mode` в правом верхнем углу (это необходимо для загрузки текстуры карты)
- Выбрать "SkyBoxes": элементы карты, которые находятся далеко от нее
- Перейти в `Object->Transform VMF 3D Sky` (Объект размещен в меню Help/Pipeline)
- Также удалить сцены разминки (при необходимости)
- Сохранить результат как файл Blender в какой-нибудь директории (например, `de_mirage.blend`)
- После этого (!!!) извлечь текстуру из карты:
- Выделить все объекты на сцене
    * Зайти в `File->External Data->Unpack Resources`
    * Выбрать `Write files to current directory` (это не сработает, если `*.blend` файл не был сохранен ранее)
    * *Также обратите внимание, что некоторые текстуры могут быть неправильно извлечены, и для их получения может
      потребоваться `VTFEdit`

- `Save as` *.blend файл и в меню сохранения откройте панель предпочтений (шестеренка в правом верхнем углу),
  установите флажок `Сжать.` **Это удалит все текстуры из файла `blend` (текстуры теперь хранятся в каталоге рядом с
  файлом `blend`) и сжимает его, что значительно уменьшает размер**

- Экспортировать карту в формате FBX в ту же директорию, где был сохранен файл *.blend (все конфигурации по умолчанию).
  Не изменяйте Масштаб здесь (!!!), так как это было учтено при импорте VMF
- В меню импорта измените следующие параметры:
    * `Hierarchy Type` изменить на `Create Level Actors`
    * Установите флажок `Create Content Folder Hierarchy`
    * Удалить флажок связанный с skeletal meshes (если не нужно)

### 4. Импорт в UE4 и постобработка

- Скрипты должны выполняться в следующем порядке:
- Создайте уровень в UE4 и импортируйте этот FBX в него с помощью `File->Import Into Level`
- Исправьте статические коллизии с
  помощью [FixStaticActorsCollisions.py](../../Content/Python/Map/FixStaticActorsCollisions.py)
    * По умолчанию все коллизии, сгенерированные UE4 при импорте, неправильны (ChatGPT kinda polite)
      (похоже, из-за другой последовательности обхода вершин CS:GO). Скрипт преобразует все коллизии
      в сложные и удаляет геометрию с полностью неподдерживаемыми коллизиями.
- Исправьте коллизии динамических объектов с
  помощью [FixPhysicsPropsCollisions.py](../../Content/Python/Map/FixPhysicsPropsCollisions.py)
- Исправьте освещение окружения на карте с
  помощью [FixEnvironmentLight.py](../../Content/Python/Map/FixEnvironmentLight.py)
  * Импортированное освещение `light_environment` должно быть заменено, так как оно статическое и не имеет теней и т.д.
- Исправьте источники света на карте с помощью FixLightSpotsSources.py
  * Весь свет являются статическими, кроме Стационарного, поэтому, если требуются тени и хорошее освещение, необходимо
    исправить все actor'ы `light_*`
  * Импортированные источники света обычно слишком яркие, примерно в 10 раз ярче нормы
- Исправить непрозрачность материалов с текстурами с альфа-каналом с помощью [FixDecalsOverlays.py](../../Content/Python/Map/FixDecalsOverlays.py)
  - `Blend mode` должен быть установлен на `Translucent` в узле материала
  - Альфа-канал текстуры должен быть подключен к `Opaque pin`
  - См. также https://docs.unrealengine.com/5.0/en-US/using-transparency-in-unreal-engine-materials/
- Удалите "плохие" TriMesh с помощью [FixFailedTriMesh.py](../../Content/Python/Map/FixFailedTriMesh.py)
    * Некоторые Mesh'и недействительны, и UE4 не может их правильно обработать (UE4 выведет это в файле журнала)
    * Сохраните файл журнала с ошибками 'Failed to cook TriMesh' в виде `<map_name>.txt`
      в [fixups directory](../../Importing/fixups)

P.S.: Скрипты разделены между собой, и каждый раз мы проходим по всем актерам, чтобы возможно было их запускать
независимо.

### 6. Каталог импортированного материала в UE4