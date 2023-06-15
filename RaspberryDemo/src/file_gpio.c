#include "file_gpio.h"
int
GPIOExport(int pin)
{
#define BUFFER_MAX 3
    char buffer[BUFFER_MAX];
    ssize_t bytes_written;
    int fd;

    fd = open("/sys/class/gpio/export", O_WRONLY);
    if (-1 == fd) {
        fprintf(stderr, "Failed to open export for writing!\n");
        return(-1);
    }

    bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
    write(fd, buffer, bytes_written);
    close(fd);
    return(0);
}

int
GPIOUnexport(int pin)
{
    char buffer[BUFFER_MAX];
    ssize_t bytes_written;
    int fd;

    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (-1 == fd) {
        fprintf(stderr, "Failed to open unexport for writing!\n");
        return(-1);
    }

    bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
    write(fd, buffer, bytes_written);
    close(fd);
    return(0);
}

int
GPIODirection(int pin, int dir)
{
    static const char s_directions_str[]  = "in\0out";

#define DIRECTION_MAX 35
    char path[DIRECTION_MAX];
    int fd;

    snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", pin);
    fd = open(path, O_WRONLY);
    if (-1 == fd) {
        fprintf(stderr, "Failed to open gpio direction for writing!\n");
        return(-1);
    }

    if (-1 == write(fd, &s_directions_str[IN == dir ? 0 : 3], IN == dir ? 2 : 3)) {
        fprintf(stderr, "Failed to set direction!\n");
        return(-1);
    }

    close(fd);
    return(0);
}

int
GPIORead(int pin)
{
#define VALUE_MAX 30
    char path[VALUE_MAX];
    char value_str[3];
    int fd;

    snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
    fd = open(path, O_RDONLY);
    if (-1 == fd) {
        fprintf(stderr, "Failed to open gpio value for reading!\n");
        close(fd);
        return(-1);
    }

    if (-1 == read(fd, value_str, 3)) {
        fprintf(stderr, "Failed to read value!\n");
        close(fd);
        return(-1);
    }

    close(fd);

    return(atoi(value_str));
}

int
GPIOWrite(int pin, int value)
{
    static const char s_values_str[] = "01";

    char path[VALUE_MAX];
    int fd;

    snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
    fd = open(path, O_WRONLY);
    if (-1 == fd) {
        fprintf(stderr, "Failed to open gpio value for writing!\n");
        close(fd);
        return(-1);
    }

    if (1 != write(fd, &s_values_str[LOW == value ? 0 : 1], 1)) {
        fprintf(stderr, "Failed to write value!\n");
        close(fd);
        return(-1);
    }

    close(fd);
    return(0);
}

int relay_init()
{
    if (!bcm2835_init())
    {
        printf("bcm2835_init failed. Are you running as root??\n");
        return -1;
    }
    /* GPIO Init*/
    bcm2835_gpio_fsel(WARN_LED_PIN   , BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(SENSOR_OK_PIN  , BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(PWR_5V_OK_PIN  , BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(PWR_15V_OK_PIN , BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(SQR_SIN_SEL_PIN, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(PWR_15V_SW_PIN , BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(WARN_LED_PIN   , LOW);
    bcm2835_gpio_write(SENSOR_OK_PIN  , LOW);
    bcm2835_gpio_write(PWR_5V_OK_PIN  , LOW);
    bcm2835_gpio_write(PWR_15V_OK_PIN , LOW);
    bcm2835_gpio_write(SQR_SIN_SEL_PIN, LOW);
    bcm2835_gpio_write(PWR_15V_SW_PIN , LOW);
    bcm2835_delay(200);
    return 0;
}

void sensor_ok_on()
{
    bcm2835_gpio_write(SENSOR_OK_PIN , HIGH);
}

void sensor_ok_off()
{
    bcm2835_gpio_write(SENSOR_OK_PIN , LOW);
}

void pwr_15v_ok_on()
{
    bcm2835_gpio_write(PWR_15V_OK_PIN , HIGH);
}

void pwr_15v_ok_off()
{
    bcm2835_gpio_write(PWR_15V_OK_PIN , LOW);
}

void toggle_warning(int ms_delay)
{
    bcm2835_gpio_write(WARN_LED_PIN , HIGH);
    bcm2835_delay(ms_delay);
    bcm2835_gpio_write(WARN_LED_PIN , LOW);
}

void pwr_5v_ok_on()
{
    bcm2835_gpio_write(PWR_5V_OK_PIN , HIGH);
}

void pwr_5v_ok_off()
{
    bcm2835_gpio_write(PWR_5V_OK_PIN , LOW);
}

void pwr_15v_on()
{
    bcm2835_gpio_write(PWR_15V_SW_PIN , HIGH);
}

void pwr_15v_off()
{
    bcm2835_gpio_write(PWR_15V_SW_PIN , LOW);
}

void relays_off()
{
    bcm2835_gpio_write(WARN_LED_PIN   , LOW);
    bcm2835_gpio_write(SENSOR_OK_PIN  , LOW);
    bcm2835_gpio_write(PWR_5V_OK_PIN  , LOW);
    bcm2835_gpio_write(PWR_15V_OK_PIN , LOW);
    bcm2835_gpio_write(SQR_SIN_SEL_PIN, LOW);
    bcm2835_gpio_write(PWR_15V_SW_PIN , LOW);
}
