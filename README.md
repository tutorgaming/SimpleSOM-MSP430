# SimpleSOM-MSP430
Low level Storage Of Simple Self-Organzing Map (Kohonen Map)
on Microcontroller 

This is the second task on ICCS internship program
at TU-Chemnitz , Germany

by Theppasith Nisitsukcharoen

Third-year Computer Engineering student , Chulalongkorn University

### Requirement

**Desktop Side**
1. Source-Code here [SimpleSOM-junkie](https://github.com/Tutorgaming/SimpleSOM-junkie) 
2. Windows 7 (According to serial transmission Library)
3. SFML Library - Draw Visualization 
  - [SFML](http://www.sfml-dev.org/download.php) -Please Select 32-Bit - code:block set path to = C:\SFML-2.3
4. CODE::BLOCK (IDE for C++/C)  - Project files formatted for code:Block
  - [CODE:BLOCK](http://www.codeblocks.org/downloads)

**Microcontroller Side**

1. MSP430F5529 Board (Texas instrument)
2. Code Composer Studio (Texas instument modded version of Eclipse) 

### Setting it up
* First, you have to clone this repo into your computer.
* You can use SOURCETREE or any git clients you want to.
```sh 
   $ cd path/to/your/workspace
   $ git clone https://github.com/Tutorgaming/SimpleSOM-MSP430.git
```
* Then Open the project file using  Code Composer Studio [.ccsproject]
* Use the debug button inside Code Composer Studio to Complie
* and you need to press Play Button to RUN program manually 

### Workflow

1. Microcontroller will receive the trained Self-Organizing Networks from the Desktop side
2. Also the Plotter Matrix in order to find the specific class for newly input data
3. After receiving all inputs and parameters, it waits for newly input data from Desktop side 
4. Next, the test input data are classify and showns as the number of blinking LED 
5. Finally, it waits for next input.
