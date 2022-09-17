#include<iostream>
#include<vector>
#include<thread>
#include"color.h"
#include"ray.h"
#include"vec3.h"
#include"utility.h"
#include"hittable_list.h"
#include"sphere.h"
#include"camera.h"

const auto aspect_ratio = 16.0 / 9.0;
const int image_width = 900;
const int image_height = static_cast<int>(image_width / aspect_ratio);
const int samples_per_pixel = 50;
const int max_depth = 50;

//camera
camera cam;

//world
hittable_list world;

color ray_color(const ray& r, const hittable& world, int depth)
{
	hit_record rec;

	//checking ray bounce limit
	if (depth <= 0)
	{
		return color(0, 0, 0);
	}
	if (world.hit(r, 0.001, infinity, rec))
	{
		point3 target = rec.p + rec.normal + random_in_unit_sphere();
		return 0.5 * ray_color(ray(rec.p, target - rec.p), world, depth - 1);
	}

	vec3 unit_direction = unit_vector(r.direction());

	//for vertical gradient
	auto t = 0.5 * (unit_direction.y() + 1.0);	
	return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

static void calculateScanline(std::vector<std::vector<color>>& outputBuffer, int startRowNum, int endRowNum)
{
	for (int j = startRowNum; j >= endRowNum; j--)
	{
		for (int i = 0; i < image_width; i++)
		{
			color pixel_color(0, 0, 0);
			for (int s = 0; s < samples_per_pixel; s++)
			{
				auto u = (i + random_double()) / (image_width - 1);
				auto v = (j + random_double()) / (image_height - 1);
				ray r = cam.get_ray(u, v);
				pixel_color += ray_color(r, world, max_depth);
			}

			write_color(outputBuffer, pixel_color, samples_per_pixel, j, i);
		}
	}
}

int main()
{
	std::vector<std::vector<color>> outputBuffer(image_height, std::vector<color>(image_width));
		
	world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
	world.add(make_shared<sphere>(point3(0, 1, -1), 0.5));

	world.add(make_shared<sphere>(point3(1, 1, -1), 0.5));
	world.add(make_shared<sphere>(point3(-1, 1, -1), 0.5));

	world.add(make_shared<sphere>(point3(-1, 0, -1), 0.5));
	world.add(make_shared<sphere>(point3(1, 0, -1), 0.5));				//main sphere

	world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));			//groundvisual

	//int numThreads = std::thread::hardware_concurrency()*2;
	int numThreads=12;
	//int numThreads=16;

	std::vector<std::thread> threadVec;
	int rowsPerThread = image_height / numThreads;

	//render
	std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

	int rowCount = image_height - 1;
	for (int y = 0; y < numThreads; y++)
	{
		if (y == numThreads - 1)
		{
			std::thread th(calculateScanline, std::ref(outputBuffer), rowCount, 0);
			threadVec.push_back(std::move(th));
			rowCount -= rowsPerThread;

		}
		else
		{
			std::thread th(calculateScanline, std::ref(outputBuffer), rowCount, rowCount - rowsPerThread);
			threadVec.push_back(std::move(th));
			rowCount -= rowsPerThread;
		}
	}

	int count = 1;
	for (auto& t : threadVec)
	{
		if (t.joinable())
		{
			t.join();
		}
		std::cerr << "\rThreads Done:  " << count << " " << std::flush;
		count++;
	}

	for (int j = image_height - 1; j >= 0; j--)
	{
		for (int i = 0; i < image_width; i++)
		{
			std::cout << outputBuffer[j][i].e[0] << " " << outputBuffer[j][i].e[1] << " " << outputBuffer[j][i].e[2] << "\n";
		}
	}
	
	std::cerr << "\ndone.\n";
}
