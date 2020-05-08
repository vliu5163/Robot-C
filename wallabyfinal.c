#include <kipr/botball.h>

#define LEFT_TOPHAT 5
#define F_L_TOPHAT 1
#define F_R_TOPHAT 0
#define F_THRESHOLD 3500
#define L_THRESHOLD 1000


#define LEFT_MOTOR 3
#define RIGHT_MOTOR 0

#define H_SERVO 2
#define LIFT_ROBOT 650
#define LIFT_SERVO 1
#define RIGHT_SERVO 0
#define LEFT_SERVO 3
#define RIGHT_OPEN 0
#define RIGHT_CLOSED 600
#define LEFT_OPEN 2047
#define LEFT_CLOSED 1500
#define LIFT_DOWN 2047
#define LIFT_UP 1400
#define LIFT_UP_UP 500


#define ssp set_servo_position

int yellow_zone = 0;

void camera()
{
    camera_open();
    int i = 0;
    int num = 0;
    for(i = 0; i < 50; i++)
    {
        camera_update();
    }
    for(i = 0; i < 50; i++)
    {
        camera_update();
        num += get_object_count(0);
    }
    num /= 50;
    if(num > 0)
    {
        yellow_zone = 1;
    }
    camera_close();
}

void move_to_crates()
{
    //move to line 
    int i = 0;
    int old_reading = analog(LEFT_TOPHAT);
    mav(RIGHT_MOTOR, 1500);
    mav(LEFT_MOTOR, 1500);
    while (i < 2)
    {
        msleep(100);
        int new_reading = analog(LEFT_TOPHAT);
        if (new_reading - old_reading > L_THRESHOLD)
        {
            i++;
        }
        old_reading = new_reading;
    }
    ao();

    //turn the robot right
    //front left little sensor: 1, 2, 3
    //front right little sensor: 0, 4
    //side top hat: 5
    mav(LEFT_MOTOR,1500);
    while(analog(F_R_TOPHAT) < F_THRESHOLD);
    while(analog(F_R_TOPHAT) > F_THRESHOLD);
    ao();

    // double start = seconds();
    thread tid;
    tid = thread_create(camera);
    thread_start(tid);
    while(analog(LEFT_TOPHAT) < L_THRESHOLD)
    {   
        if(analog(F_R_TOPHAT) < F_THRESHOLD)
        {
            mav(LEFT_MOTOR,1000);
            mav(RIGHT_MOTOR,1500);
        }
        else if(analog(F_R_TOPHAT) > F_THRESHOLD)
        {
            mav(RIGHT_MOTOR,1000);
            mav(LEFT_MOTOR,1500);
        }
    }
    msleep(500);
    ao();
    thread_destroy(tid);
    camera_open();
    for(i = 0; i < 50; i++)
    {
        camera_update();
    }
    if(yellow_zone<1)
    {
        camera_update();
        if(get_object_count(0) >= 1)
            yellow_zone = 2;
        else
            yellow_zone = 3;
        camera_close();
    }
        /*if (seconds() - start < 3 && yellow_zone < 1)
        {
	        camera_update();
            if(get_object_count(0) > 0)
	            yellow_zone = 1;
        }
    }
    camera_update();
    if (get_object_count(0) >= 1 && yellow_zone != 1)
    {
        yellow_zone = 2;
    }
    else
    {
        yellow_zone = 3;
    }
    printf("%d", yellow_zone);*/
    //turn robot left
    mav(RIGHT_MOTOR, 1500);
    mav(LEFT_MOTOR, -1500);
    msleep(50);
    while(analog(F_L_TOPHAT) > F_THRESHOLD);
    while(analog(F_L_TOPHAT) < F_THRESHOLD);
    while(analog(F_L_TOPHAT) > F_THRESHOLD);
    msleep(150);
    mav(RIGHT_MOTOR, -500);
    mav(LEFT_MOTOR, 500);
    while(analog(F_L_TOPHAT) < F_THRESHOLD);
    ao();

    //go forward
    double start = seconds();
    while(seconds()-start < 1.5)
    {
        if(analog(F_R_TOPHAT) < F_THRESHOLD)
        {
            mav(LEFT_MOTOR,1000);
            mav(RIGHT_MOTOR,1500);
        }
        else if(analog(F_R_TOPHAT) > F_THRESHOLD)
        {
            mav(RIGHT_MOTOR,1000);
            mav(LEFT_MOTOR,1500);
        }
    }
    ao();
}

void pick_up_crates()
{
    ssp(RIGHT_SERVO, RIGHT_CLOSED);
    ssp(LEFT_SERVO, LEFT_CLOSED);
    msleep(1000);
    ssp(LIFT_SERVO, LIFT_UP);
    msleep(1000);
    mav(RIGHT_MOTOR, -1500);
    mav(LEFT_MOTOR, -1500);
    while(analog(LEFT_TOPHAT)<L_THRESHOLD);
    while(analog(LEFT_TOPHAT)>L_THRESHOLD);
    mav(LEFT_MOTOR, -1300);
    msleep(500);
    ao();
}

void close_zone()
{
    msleep(1000);
    mav(LEFT_MOTOR, -1500);
    msleep(2000);
    mav(RIGHT_MOTOR, 1500);
    mav(LEFT_MOTOR, 1500);
    msleep(3500);
    ao();
    mav(RIGHT_MOTOR, 1500);
    mav(LEFT_MOTOR, -1500);
    msleep(1000);
    // square up
    while (analog(F_L_TOPHAT) < F_THRESHOLD || analog(F_R_TOPHAT) < F_THRESHOLD)
    {
        if(analog(F_L_TOPHAT) > F_THRESHOLD)
        {
            mav(RIGHT_MOTOR, 1500);
            mav(LEFT_MOTOR, -500);
            printf("a\n");
        }
        else if(analog(F_R_TOPHAT) > F_THRESHOLD)
        {
            mav(LEFT_MOTOR, 1500);
            mav(RIGHT_MOTOR, -500);
            printf("b\n");
        }
        else
        {
            mav(LEFT_MOTOR, 1500);
            mav(RIGHT_MOTOR, 1500);
            printf("c\n");
        }
        msleep(10);
    }
    ao();
    
    ssp(LIFT_SERVO, LIFT_DOWN);
    ssp(LEFT_SERVO, LEFT_OPEN);
    ssp(RIGHT_SERVO, RIGHT_OPEN);
    
    mav(RIGHT_MOTOR, -500);
    mav(LEFT_MOTOR, -500);
    msleep(1000);
    ssp(LIFT_SERVO, LIFT_DOWN);
    mav(RIGHT_MOTOR, 500);
    mav(LEFT_MOTOR, 500);
    msleep(1000);
    ao();
    ssp(LIFT_SERVO, LIFT_UP);
    msleep(500);
    ssp(LEFT_SERVO, LEFT_CLOSED);
    ssp(RIGHT_SERVO, RIGHT_CLOSED);
    msleep(500);
    ssp(LIFT_SERVO, LIFT_UP_UP);
    msleep(1000);
    mav(RIGHT_MOTOR, -1000);
    mav(LEFT_MOTOR, -1000);
    msleep(1500);
    ao();
    mav(RIGHT_MOTOR, -1500);
    msleep(1500);
	while (analog(F_L_TOPHAT) < F_THRESHOLD || analog(F_R_TOPHAT) < F_THRESHOLD)
    {
        if(analog(F_L_TOPHAT) > F_THRESHOLD)
        {
            mav(RIGHT_MOTOR, 1000);
            mav(LEFT_MOTOR, -500);
            printf("a\n");
        }
        else if(analog(F_R_TOPHAT) > F_THRESHOLD)
        {
            mav(LEFT_MOTOR, 1000);
            mav(RIGHT_MOTOR, -500);
            printf("b\n");
        }
        else
        {
            mav(LEFT_MOTOR, 1500);
            mav(RIGHT_MOTOR, 1500);
            printf("c\n");
        }
        msleep(10);
    }
    ao();
    msleep(1000);
    mav(LEFT_MOTOR, -1500);
    msleep(1500);
    ao();
    msleep(1000);
    ssp(LIFT_SERVO, LIFT_DOWN);
    while (analog(F_L_TOPHAT) < F_THRESHOLD || analog(F_R_TOPHAT) < F_THRESHOLD)
    {
        if(analog(F_L_TOPHAT) > F_THRESHOLD)
        {
            mav(RIGHT_MOTOR, 1500);
            mav(LEFT_MOTOR, -500);
            printf("a\n");
        }
        else if(analog(F_R_TOPHAT) > F_THRESHOLD)
        {
            mav(LEFT_MOTOR, 1500);
            mav(RIGHT_MOTOR, -500);
            printf("b\n");
        }
        else
        {
            mav(LEFT_MOTOR, 1500);
            mav(RIGHT_MOTOR, 1500);
            printf("c\n");
        }
        msleep(10);
    }
    ao();
    msleep(1000);
    ssp(LEFT_SERVO, LEFT_OPEN);
    ssp(RIGHT_SERVO, RIGHT_OPEN);
    msleep(1000);
    //mav(LEFT_MOTOR, 1500);
    //mav(RIGHT_MOTOR, 1500);
    mav(LEFT_MOTOR, -1500);
    mav(RIGHT_MOTOR, -1500);
    msleep(500);
    mav(LEFT_MOTOR, 1500);
    mav(RIGHT_MOTOR, 1500);
    msleep(750);
	mav(LEFT_MOTOR, -1500);
    mav(RIGHT_MOTOR, -1500);
    msleep(1000);
	ao();
}

void middle_zone()
{
    msleep(1000);
    mav(RIGHT_MOTOR, -1500);
    msleep(3750);
    while (analog(F_L_TOPHAT) < F_THRESHOLD || analog(F_R_TOPHAT) < F_THRESHOLD)
    {
        if(analog(F_L_TOPHAT) > F_THRESHOLD)
        {
            mav(RIGHT_MOTOR, 1500);
            mav(LEFT_MOTOR, -500);
            printf("a\n");
        }
        else if(analog(F_R_TOPHAT) > F_THRESHOLD)
        {
            mav(LEFT_MOTOR, 1500);
            mav(RIGHT_MOTOR, -500);
            printf("b\n");
        }
        else
        {
            mav(LEFT_MOTOR, 1500);
            mav(RIGHT_MOTOR, 1500);
            printf("c\n");
        }
        msleep(10);
    }
    ao();
    ssp(LIFT_SERVO, LIFT_DOWN);
    ssp(LEFT_SERVO, LEFT_OPEN);
    ssp(RIGHT_SERVO, RIGHT_OPEN);
    msleep(500);
    mav(RIGHT_MOTOR, -500);
    mav(LEFT_MOTOR, -500);
    msleep(1000);
    ssp(LIFT_SERVO, LIFT_DOWN);
    mav(RIGHT_MOTOR, 500);
    mav(LEFT_MOTOR, 500);
    msleep(1000);
    ao();
    ssp(LIFT_SERVO, LIFT_UP);
    msleep(500);
    ssp(LEFT_SERVO, LEFT_CLOSED);
    ssp(RIGHT_SERVO, RIGHT_CLOSED);
    msleep(500);
    ssp(LIFT_SERVO, LIFT_UP_UP);
    msleep(1000);
    mav(RIGHT_MOTOR, -1000);
    mav(LEFT_MOTOR, -1000);
    msleep(1500);
    ao();
    mav(LEFT_MOTOR, -1500);
    mav(RIGHT_MOTOR, 1500);
    msleep(1000);
    mav(LEFT_MOTOR, 1500);
    msleep(2500);
    mav(RIGHT_MOTOR, -1500);
    msleep(1000);
    ao();
    ssp(LIFT_SERVO, LIFT_DOWN);
    while (analog(F_L_TOPHAT) < F_THRESHOLD || analog(F_R_TOPHAT) < F_THRESHOLD)
    {
        if(analog(F_L_TOPHAT) > F_THRESHOLD)
        {
            mav(RIGHT_MOTOR, 1500);
            mav(LEFT_MOTOR, -500);
            printf("a\n");
        }
        else if(analog(F_R_TOPHAT) > F_THRESHOLD)
        {
            mav(LEFT_MOTOR, 1500);
            mav(RIGHT_MOTOR, -500);
            printf("b\n");
        }
        else
        {
            mav(LEFT_MOTOR, 1500);
            mav(RIGHT_MOTOR, 1500);
            printf("c\n");
        }
        msleep(10);
    }
    ao();
    msleep(1000);
    ssp(LEFT_SERVO, LEFT_OPEN);
    ssp(RIGHT_SERVO, RIGHT_OPEN);
    msleep(1000);
    mav(RIGHT_MOTOR, -1500);
    mav(LEFT_MOTOR, -1500);
    msleep(1500);
    mav(LEFT_MOTOR, 1500);
    msleep(1000);
    mav(RIGHT_MOTOR, 1500);
    msleep(6500);
    ao();
}

void far_zone()
{
    // if furthest zone
    msleep(1000);
    mav(RIGHT_MOTOR, -1500);
    //mav(LEFT_MOTOR, 1500);
    msleep(2000);
    mav(RIGHT_MOTOR, 1500);
    mav(LEFT_MOTOR, 1500);
    msleep(4500);
    ao();

    mav(RIGHT_MOTOR, -1500);
    mav(LEFT_MOTOR, 1500);
    msleep(1150);
    // square up
    while (analog(F_L_TOPHAT) < F_THRESHOLD || analog(F_R_TOPHAT) < F_THRESHOLD)
    {
        if(analog(F_L_TOPHAT) > F_THRESHOLD)
        {
            mav(RIGHT_MOTOR, 1500);
            mav(LEFT_MOTOR, -500);
            printf("a\n");
        }
        else if(analog(F_R_TOPHAT) > F_THRESHOLD)
        {
            mav(LEFT_MOTOR, 1500);
            mav(RIGHT_MOTOR, -500);
            printf("b\n");
        }
        else
        {
            mav(LEFT_MOTOR, 1500);
            mav(RIGHT_MOTOR, 1500);
            printf("c\n");
        }
        msleep(10);
    }
    ao();
    
    ssp(LIFT_SERVO, LIFT_DOWN);
    ssp(LEFT_SERVO, LEFT_OPEN);
    ssp(RIGHT_SERVO, RIGHT_OPEN);
    
    mav(RIGHT_MOTOR, -500);
    mav(LEFT_MOTOR, -500);
    msleep(1000);
    ssp(LIFT_SERVO, LIFT_DOWN);
    mav(RIGHT_MOTOR, 500);
    mav(LEFT_MOTOR, 500);
    msleep(1000);
    ao();
    ssp(LIFT_SERVO, LIFT_UP);
    msleep(500);
    ssp(LEFT_SERVO, LEFT_CLOSED);
    ssp(RIGHT_SERVO, RIGHT_CLOSED);
    msleep(500);
    ssp(LIFT_SERVO, LIFT_UP_UP);
    msleep(1000);
    mav(RIGHT_MOTOR, -1000);
    mav(LEFT_MOTOR, -1000);
    msleep(1500);
    ao();
    mav(LEFT_MOTOR, -1500);
    mav(RIGHT_MOTOR, 1500);
    msleep(1000);
    ao();
    // square up
    while (analog(F_L_TOPHAT) < F_THRESHOLD || analog(F_R_TOPHAT) < F_THRESHOLD)
    {
        if(analog(F_L_TOPHAT) > F_THRESHOLD)
        {
            mav(RIGHT_MOTOR, 1000);
            mav(LEFT_MOTOR, -500);
            printf("a\n");
        }
        else if(analog(F_R_TOPHAT) > F_THRESHOLD)
        {
            mav(LEFT_MOTOR, 1000);
            mav(RIGHT_MOTOR, -500);
            printf("b\n");
        }
        else
        {
            mav(LEFT_MOTOR, 1500);
            mav(RIGHT_MOTOR, 1500);
            printf("c\n");
        }
        msleep(10);
    }
    ao();
    msleep(1000);
    mav(RIGHT_MOTOR, -1500);
    msleep(1500);
    ao();
    msleep(1000);
    ssp(LIFT_SERVO, LIFT_DOWN);
    while (analog(F_L_TOPHAT) < F_THRESHOLD || analog(F_R_TOPHAT) < F_THRESHOLD)
    {
        if(analog(F_L_TOPHAT) > F_THRESHOLD)
        {
            mav(RIGHT_MOTOR, 1500);
            mav(LEFT_MOTOR, -500);
            printf("a\n");
        }
        else if(analog(F_R_TOPHAT) > F_THRESHOLD)
        {
            mav(LEFT_MOTOR, 1500);
            mav(RIGHT_MOTOR, -500);
            printf("b\n");
        }
        else
        {
            mav(LEFT_MOTOR, 1500);
            mav(RIGHT_MOTOR, 1500);
            printf("c\n");
        }
        msleep(10);
    }
    ao();
    msleep(1000);
    ssp(LEFT_SERVO, LEFT_OPEN);
    ssp(RIGHT_SERVO, RIGHT_OPEN);
    msleep(1000);
    //mav(LEFT_MOTOR, 1500);
    //mav(RIGHT_MOTOR, 1500);
    mav(LEFT_MOTOR, -1500);
    mav(RIGHT_MOTOR, -1500);
    msleep(500);
    mav(LEFT_MOTOR, 1500);
    mav(RIGHT_MOTOR, 1500);
    msleep(750);
    mav(LEFT_MOTOR, -1500);
    mav(RIGHT_MOTOR, -1500);
    msleep(5000);
}

int main()
{
    ssp(LIFT_SERVO, LIFT_DOWN);
    ssp(RIGHT_SERVO, RIGHT_OPEN);
    ssp(LEFT_SERVO, LEFT_OPEN);
    //wait_for_light(4);
    double start = seconds();
    shut_down_in(119);
    enable_servos();
    move_to_crates();
    pick_up_crates();
    if(yellow_zone == 1)
    { 
        close_zone();
    }
    else if(yellow_zone == 2)
    {
    	middle_zone();
    }
    else if(yellow_zone == 3)
    {
    	far_zone();
    }
    printf("TIME TAKEN: %f s\n\n", seconds() - start);
    return 0;
}
