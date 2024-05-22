## CSC615 Group Term Project

### Drive On

Have all your code in the respository along with videos (or links to videos)

Team Report - PDF (also submitted to Canvas)

Should have title page and each page should have your team name, date, and class in the header and page number in the footer.

- List each team member, and Github Username of primary github with your full source and Makefile 
- Task Description
- Building the Robot (include photos)
- Parts / Sensors Used (include photo, and part numbers where applicable, such as HC-SR04 for the sonic echo sensor)
- How was bot built (photos good to include)
- What libraries/software did you use in your code (include full reference)
- Flowchart of your code
- Pin Assignments you used
- Hardware Diagram
- What worked well
- What were issues 

Optional Appendix:

- Listing of all your code
- Makefile

In addition to this will be the final running of your bot which will be recorded and an Individual submission about your team and experience.

```mermaid
flowchart TD
    A[Start] --> B[Initialize system modules]
    B --> |Initialization fails| C[Exit]
    B --> |Initialization succeeds| D[Initialize GPIO library]
    D --> |Initialization fails| E[Return 1]
    D --> |Initialization succeeds| F[Register signal handlers]
    F --> G[Configure button pin]
    G --> H[Enable pull-up resistor]
    H --> I[Print Waiting for button press...]
    I --> J{Button Pressed?}
    J --> |No| K[Read button state]
    K --> J
    J --> |Yes| L[Set buttonPressed to 1]
    L --> M[Configure GPIO pins]
    M --> N[Initialize sensors and motor driver]
    N --> O[Create and start threads for sensors]
    O --> P{Cleaned up?}
    P --> |No| Q[Print front obstacle distance]
    Q --> |Obstacle detected| R[Avoid obstacle]
    Q --> |No obstacle| S{Sensors detect line?}
    S --> |Both clear| T[Move forward]
    S --> |Left line detected| U[Turn right]
    S --> |Right line detected| V[Turn left]
    P --> |Yes| W[Wait for threads to finish]
    W --> X[Return 0]
    
    R --> R1[Stop motors]
    R1 --> R2[Turn to side]
    R2 --> R3{Side obstacle distance > 13?}
    R3 --> |Yes| R4[Turn car distance]
    R3 --> |No| R5{Side obstacle distance < 50?}
    R5 --> |Yes| R6[Move forward]
    R5 --> |No| R7[Avoid turn 1st corner]
    R7 --> R8[Small wait for pins]
    R8 --> R9[Move forward]
    R9 --> R10{Side obstacle distance < 44?}
    R10 --> |Yes| R11[Move forward]
    R10 --> |No| R12[Avoid turn 2nd corner]
    R12 --> R13[Move back in line]

    classDef startEnd fill:#f9f,stroke:#333,stroke-width:4px;
    class A,C,E,X startEnd;

