#define PRESSURE_SENSOR 0

/* 
 * returns the pressure differential between the outside of the balloon
 * and the inside in kPa
 */
double pressureDifferential() {
  int v = analogRead(PRESSURE_SENSOR);
  double pressure = (double) v / 9.0 - 4.444;
  return pressure;
}
