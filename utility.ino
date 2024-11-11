void powerDownSensors(void)
{
  digitalWrite(SENSOR_PWR, LOW);
}

void powerUpSensors(void)
{
  delay(500);
  digitalWrite(SENSOR_PWR, HIGH);
  delay(500);
}
