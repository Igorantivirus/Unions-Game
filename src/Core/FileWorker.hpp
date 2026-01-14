#pragma once

#include <string>

#include <SDL3/SDL_iostream.h>

namespace sdl3io
{

enum class FileWorkerMode : unsigned char
{
    read = 0b00000001,   // чтение
    write = 0b00000010,  // запись
    app = 0b00000100,    // добавление
    binary = 0b00001000, // бинарно
    dblmode = 0b00010000 // чтение + запись
};

inline static FileWorkerMode operator&(const FileWorkerMode left, const FileWorkerMode right)
{
    return static_cast<FileWorkerMode>(static_cast<unsigned char>(left) & static_cast<unsigned char>(right)); 
}

inline static FileWorkerMode operator|(const FileWorkerMode left, const FileWorkerMode right)
{
    return static_cast<FileWorkerMode>(static_cast<unsigned char>(left) | static_cast<unsigned char>(right)); 
}

class FileWorker
{
public:
    FileWorker() = default;
    FileWorker(const std::string_view path, const FileWorkerMode mode)
    {
        open(path, mode);
    }
    FileWorker(const std::string_view path, const int mode)
    {
        open(path, mode);
    }
    ~FileWorker()
    {
        close();
    }

    bool open(const std::string_view path, const int mode)
    {
        return open(path, static_cast<FileWorkerMode>(mode));
    }
    bool open(const std::string_view path, const FileWorkerMode mode)
    {
        char strMode[4] = {};
        fillMode(strMode, mode);
        io_ = SDL_IOFromFile(path.data(), strMode);
        mode_ = mode;
        return static_cast<bool>(io_);
    }

    bool close()
    {
        mode_ = static_cast<FileWorkerMode>(0);
        if (io_)
            return SDL_CloseIO(io_);
        return false;
    }

    bool isOpen() const
    {
        return static_cast<bool>(io_);
    }

    std::string readAll()
    {
        const auto sz = SDL_GetIOSize(io_);
        std::string buf(static_cast<size_t>(sz), '\0');
        SDL_ReadIO(io_, buf.data(), buf.size());
        return buf;
    }
    std::string readNext(const Sint64 size)
    {
        std::string buf(static_cast<size_t>(size), '\0');
        SDL_ReadIO(io_, buf.data(), buf.size());
        return buf;
    }

    bool write(const std::string_view str)
    {
        size_t bytesWritten = SDL_WriteIO(io_, str.data(), str.size());
        return bytesWritten == str.size();
    }

    bool seek(long offset, const SDL_IOWhence type)
    {
        Sint64 result = SDL_SeekIO(io_, offset, type);
        return result != -1;
    }
    size_t tell()
    {
        Sint64 position = SDL_TellIO(io_);
        if (position == -1)
            return 0;
        return static_cast<size_t>(position);
    }

private:
    SDL_IOStream *io_ = nullptr;
    FileWorkerMode mode_{};

private:
    static void fillMode(char modeStr[4], const FileWorkerMode fmode)
    {
        std::memset(modeStr, 0, 4);
        unsigned char ind = 0;
        modeStr[ind++] =
        static_cast<bool>(fmode & FileWorkerMode::read)  ? 'r' :
        static_cast<bool>(fmode & FileWorkerMode::write) ? 'w' :
        static_cast<bool>(fmode & FileWorkerMode::app)   ? 'a' :
        '\0';
        if (static_cast<bool>(fmode & FileWorkerMode::binary))
            modeStr[ind++] = 'b';
        if (static_cast<bool>(fmode & FileWorkerMode::dblmode))
            modeStr[ind++] = '+';
    }
};

} // namespace sdl3io