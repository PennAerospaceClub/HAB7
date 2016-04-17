#define OPEN 0
#define CLOSED 90
#define valve 6

void altitudeControl(long alt) {
  if (alt > DESIRED_ALT) {
    valve.write(OPEN);
  } else {
    valve.write(CLOSED);
  }
}
