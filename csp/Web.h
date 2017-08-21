#pragma once

class Web
{
public:
	Web(const Web&) = delete;
	Web& operator=(const Web&) = delete;

	Web() noexcept;
	~Web() noexcept;
	void start()noexcept;
};