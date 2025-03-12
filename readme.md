
This project contains a simulator and primitive assembler for the TD4 4-bit
CPU, see <https://github.com/wuxx/TD4-4BIT-CPU> for more information, or just
look at the code in `td4.c`.

Check out the `doc/` subdirectory for the original project.

# Ideas / To Do:

I will most likely never get around to doing this, and instead just using this
space to document some ideas.

* [ ] Translate <https://github.com/wuxx/TD4-4BIT-CPU> into English.
* [ ] Get this design up and running in Proteus Design Suite / Other PCB
  and circuit simulator software.
* [ ] Tweak circuit, make improvements, such as:
  - [ ] Better power supply input / alternatives.
  - [ ] Power supply over-voltage, over-current and reverse bias
  protection <https://www.circuitbasics.com/protection-circuits>.
  - [ ] Instead of output going to some LEDs, it could also be brought
    out to an extension board.
  - [ ] Instead of DIP switches the input could be (optionally) be taken
  from an external set of input pins.
  - [ ] Could the diodes on the board be replaced by LEDs to give more
  feedback? We could certainly light up the register output and which 
  instruction is being executed.
  - [ ] Adjustable frequency control option.
  - [ ] Changes / improvements to instruction set.
  - [ ] Make the CPU more configurable using jumper cables (e.g. we could
  perhaps make the Jump On Carry into a Jump on Not Carry via a jumper,
  along with some of the instruction decoding logic which could be changed
  via jumpers as well).
  - [ ] The Holtek HT12A/HT12E Encoders and HT12D/HT12F decoders could be
  used in conjunction with some simple radio circuits to make radio
  controlled peripherals. 
* [ ] GUI simulator
* [ ] Simulator in javascript using WebGL to display an interactive version
      of the TD-4.
* [ ] Turn this into a primitive Programmable Logic Controller.
* [ ] Make more example programs
* [ ] Improve assembler (add labels, debugger).

# Links

* <https://github.com/wuxx/TD4-4BIT-CPU>
* <https://hackaday.io/project/26215-td4-cpu>
* <https://teardownit.medium.com/diy-4-bit-cpu-917e7bff228a>

Some stuff on the Richard's Controller, kind of related:

* <https://sites.ecse.rpi.edu//courses/static/ECSE-4770/LAB_3/lab3_rev_f.pdf>
* <https://www.docjava.com/cpe210/pdfs/richards.pdf>
* <https://en.wikipedia.org/wiki/Richards_controller>
