![BlinderKitten](http://tuveuxvoirunephotodemonchat.fr/blinderKittenHeader.png)

# BlinderKitten

BlinderKitten is a lighting software built on top of [JUCE framework](https://juce.com/) and the wonderful [OrganicUI](https://github.com/benkuper/juce_organicui/) by [Benjamin Kuperberg](http://benjamin.kuperberg.fr/) (thanks Ben !)
By the way, you should probably check [Chataigne](https://benjamin.kuperberg.fr/chataigne/en), a wonderful software if you don't know it !

BlinkderKitten is actually not a production software but just a personnal learning project. The current developement is dependant of my learning process :)

## How does it work ?

BlinderKitten uses some differents objects to help you create wonderful shows :
- **Channels type** : they are the different kind of DMX channels you wanna use, for example : *intensity*, *red*, *green*, *blue*, *pan*, *tilt*, etc.. They are organised in **Channel Families** like *Dimmers*, *Colors*, *Positions*... 
You can create / modify / delete types and families like you want.
- With these channels types, you can easily create **Fixture types** and use them to add and patch **Fixtures** to your show, corresponding to your stage lights.
Each fixture can have multiple **Subfixtures** in the case you have the same parameters multiple times (for example : led bars).
- **Groups** and **Presets** are reusable commands of selections or values.
- **Cuelists** contains **Cues** and allows you to run commands in the same order to act like a conductor
- **Effects** add a moving value to fixtures channels depending on a curve
- **Carousels** are a mix between Cuelists and effects. Like a cuelist, they have cues with values, but fixtures are spreaded and loop along all cues.
- **Interfaces** can me midi (to control cuelists and effects with a controller) or DMX (to output a DMX univers thru a USB-DMX node or via artnet)
- **Programmer** is an instant command on fixtures, you can copy the programmer content in groups, presets, or cuelist (actually not well implemented ^^, can be dangerous to use it)

## How to test it

### By downloading it
You can find the latest build [here](https://github.com/norbertrostaing/BlinderKitten/tree/main/Binaries/App). You'll have to download all files in the app folder and place them in the same folder on your computer.

### By compiling it yourself
Clone BlinderKitten with --recursive options to integrate organicUI module

