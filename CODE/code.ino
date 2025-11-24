#include <Servo.h>

const int num_joint = 6;

Servo joints[num_joint];
int servoPins[num_joint] = {10, 9, 11, 12, 8, 13};
int currentAngles[num_joint] = {115, 90, 155, 90, 90, 110};

String jointNames[num_joint] = {
  "BASE", "SHOULDER", "ELBOW", "FOREARM", "WRIST", "HAND"
};

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < num_joint; i++) {
    joints[i].attach(servoPins[i]);
    joints[i].write(currentAngles[i]);
  }

  Serial.println("Arm online...");
  Serial.println("Use: <JOINT> <ANGLE> <MS_PER_DEG>");
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd.length()) handleCommand(cmd);
  }
}

void moveServo(int joint, int targetAngle, int stepDelay) {
  targetAngle = constrain(targetAngle, 0, 180);

  int current = currentAngles[joint];
  int step = (targetAngle > current) ? 1 : -1;

  for (int pos = current; pos != targetAngle; pos += step) {
    joints[joint].write(pos);
    delay(stepDelay);
  }

  joints[joint].write(targetAngle);
  currentAngles[joint] = targetAngle;
}

void handleCommand(String cmd) {
  int s1 = cmd.indexOf(' ');
  int s2 = cmd.indexOf(' ', s1 + 1);

  if (s1 < 0 || s2 < 0) {
    Serial.println("You sure that command works?");
    return;
  }

  String jointName = cmd.substring(0, s1);
  int angle = cmd.substring(s1 + 1, s2).toInt();
  int speed = cmd.substring(s2 + 1).toInt();

  int idx = -1;
  for (int i = 0; i < num_joint; i++) {
    if (jointNames[i].equalsIgnoreCase(jointName)) {
      idx = i;
      break;
    }
  }

  if (idx == -1) {
    Serial.println("Hmm... can't find that joint.");
    return;
  }

  if (speed < 1) speed = 1;

  moveServo(idx, angle, speed);

  Serial.print(jointNames[idx]);
  Serial.print(": ");
  Serial.print(angle);
  Serial.print(" (");
  Serial.print(speed);
  Serial.println(" ms/deg)");
}
