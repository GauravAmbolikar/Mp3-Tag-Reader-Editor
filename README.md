# MP3 Tag Reader & Editor

A C-based **MP3 Tag Reader & Editor** for reading and modifying **ID3v2.3 metadata**.

The project allows users to view MP3 metadata and edit commonly used tags such as **Title, Artist, Album, Year, and Genre** directly from the command line.

---

## Features

* Read and display ID3v2.3 MP3 metadata
* Edit existing MP3 metadata
* Supports UTF-16LE encoded text frames
* Modify the following tags:

  * Title (`TIT2`)
  * Artist (`TPE1`)
  * Album (`TALB`)
  * Year (`TYER`)
  * Genre (`TCON`)
* Preserves the remaining MP3 data while updating a tag
* Command-line based interface
* Written completely in C

---

## Supported ID3 Tags

| Modifier | Frame ID | Description   |
| -------- | -------- | ------------- |
| `-t`     | `TIT2`   | Modify Title  |
| `-a`     | `TPE1`   | Modify Artist |
| `-A`     | `TALB`   | Modify Album  |
| `-y`     | `TYER`   | Modify Year   |
| `-g`     | `TCON`   | Modify Genre  |

---

## Requirements

* Linux / WSL
* GCC compiler
* C standard library
* MP3 file containing ID3v2.3 metadata

If you are using Windows, the project can be compiled and executed using **WSL (Windows Subsystem for Linux)**.

---

## Compilation

Open the terminal inside the project directory:

```bash
gcc *.c
```

This generates the default executable:

```text
a.out
```

---

## Usage

### 1. Display Help

```bash
./a.out -h
```

Output:

```text
Help menu for Mp3 Tag Reader and Editor:
For viewing the tags-    ./a.out -v <filename.mp3>
For editing the tags-    ./a.out -e <filename.mp3> <modifier> 

Modifier    Function
-t          Modify Title Tag
-a          Modify Artist Tag
-A          Modify Album Tag
-y          Modify Year Tag
-g          Modify Genre Tag
```

---

### 2. View MP3 Tags

Syntax:

```bash
./a.out -v <filename.mp3>
```

Example:

```bash
./a.out -v Aaj_Ki_Raat.mp3
```

Example output:

```text
Mp3 Tag Reader & Editor:
------------------------
Version ID : 2.3
Title : Aaj Ki Raat
Album : Stree 2
Year : 2024
Genre : Aaj Ki Raat (From "Stree 2")
Artist : Amitabh Bhattacharya, Sachin-Jigar, Madhubanti Ba

Extracting Album Art - Done
```

---

### 3. Edit Title

Syntax:

```bash
./a.out -e <filename.mp3> -t "<new title>"
```

Example:

```bash
./a.out -e Aaj_Ki_Raat.mp3 -t "New Title"
```

Verify the change:

```bash
./a.out -v Aaj_Ki_Raat.mp3
```

---

### 4. Edit Artist

```bash
./a.out -e Aaj_Ki_Raat.mp3 -a "New Artist"
```

---

### 5. Edit Album

```bash
./a.out -e Aaj_Ki_Raat.mp3 -A "New Album"
```

---

### 6. Edit Year

```bash
./a.out -e Aaj_Ki_Raat.mp3 -y "2026"
```

---

### 7. Edit Genre

```bash
./a.out -e Aaj_Ki_Raat.mp3 -g "Bollywood"
```

---

## How the Editor Works

The editing process follows these steps:

```text
Command Line Arguments
          ↓
    Validate Arguments
          ↓
       find_tag()
          ↓
     Find Frame ID
          ↓
      find_frame()
          ↓
 Find Frame Position & Size
          ↓
   create_new_frame()
          ↓
 Create Replacement Frame
          ↓
      rewrite_file()
          ↓
 Replace Old Frame
          ↓
     Updated MP3 File
```

---

## ID3v2.3 Frame Structure

Each ID3v2.3 frame contains:

```text
+----------------------+
| Frame ID   (4 bytes) |
+----------------------+
| Size       (4 bytes) |
+----------------------+
| Flags      (2 bytes) |
+----------------------+
| Frame Data           |
+----------------------+
```

For text frames, the frame data contains an encoding byte followed by the actual text.

This project handles **UTF-16LE encoded text frames**, which are commonly represented using:

```text
01 FF FE
```

followed by UTF-16LE character data.

---

## File Structure

The project contains the following main source files:

```text
mp3_tag_reader/
│
├── main.c
├── mp3_reader_editor.c
├── mp3_reader_editor.h
├── types.h
└── Aaj_Ki_Raat.mp3
```

### `main.c`

Handles:

* Command-line arguments
* Operation selection
* Calling viewer/editor functions
* Help menu

### `mp3_reader_editor.c`

Contains the main MP3 operations:

* File opening
* ID3 header reading
* Tag viewing
* Tag identification
* Frame searching
* New frame creation
* File rewriting

### `mp3_reader_editor.h`

Contains:

* `MP3Tags` structure
* Function declarations

### `types.h`

Contains user-defined data types and status values used by the project.

---

## Error Handling

The program handles common errors such as:

* Invalid command-line arguments
* Invalid MP3 file
* Unable to open MP3 file
* Missing ID3v2 tag
* Frame not found
* Memory allocation failure
* Temporary file creation failure

Example:

```text
Frame not found
```

is displayed when the requested ID3 frame does not exist in the MP3 file.

---

## Example Workflow

Compile the project:

```bash
gcc *.c
```

View the current tags:

```bash
./a.out -v Aaj_Ki_Raat.mp3
```

Edit the title:

```bash
./a.out -e Aaj_Ki_Raat.mp3 -t "Hello"
```

Edit the artist:

```bash
./a.out -e Aaj_Ki_Raat.mp3 -a "New Artist"
```

Edit the album:

```bash
./a.out -e Aaj_Ki_Raat.mp3 -A "My Album"
```

Edit the year:

```bash
./a.out -e Aaj_Ki_Raat.mp3 -y "2026"
```

Edit the genre:

```bash
./a.out -e Aaj_Ki_Raat.mp3 -g "Bollywood"
```

Finally, verify the updated metadata:

```bash
./a.out -v Aaj_Ki_Raat.mp3
```

---

## Technologies Used

* **Language:** C
* **Compiler:** GCC
* **Operating System:** Linux / WSL
* **Metadata Standard:** ID3v2.3
* **File Handling:** Standard C file I/O
* **Memory Management:** Dynamic memory allocation

---

## Learning Outcomes

Through this project, the following concepts are practiced:

* C programming
* Structures
* Pointers
* Dynamic memory allocation
* File handling
* Binary file processing
* Command-line arguments
* String handling
* Byte-level data manipulation
* ID3 metadata structure
* UTF-16LE encoding
* Temporary file based file modification

---

## Author

**Gaurav Ambolikar**

Electronics & Telecommunication Engineering Student
