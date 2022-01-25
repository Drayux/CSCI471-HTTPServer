## Course Affiliation

This project was written for CSCI471: Computer Networks at Colorado School of Mines.
A small portion of this project was provided as starter code to demonstrate the usage of the socket.h pipeline.

Consequently, I recommend against reusing any of this code if, by chance, you are working on the same project.
Furthermore, I request that this code be used only for non-profit and/or educational purposes. Thank you :)

#### Grade Recieved: 100 / 100

## Project Description

This is a simple HTTP server designed to impliment the required features of the project, as described in `REQUIREMENTS.pdf`. This server will respond to basic HTTP/1.0 requests for a specific set of filenames. It performs no processing of headers and is otherwise very limited in functionality. However, I still found this project rewarding, and it served as an interesting look into the basics of the backbone behind the modern internet today.

## Submission Readme

> A few notes about the project:
> Some of the requirements were a bit ambiguous, for example that the server should
> only serve files by the format of fileX.html and imageX.jpg, but also suggested
> that some testing would be performed in a web browser, which will automatically
> append a '/' to the beginning of the file requested. Thus, my server will respond
> to requests of the formats:
>  - fileX.html
>  - /fileX.html
>  - imageX.jpg
>  - /imageX.jpg
> 
> Second, there was no note as to how the requested HTTP protocol version should be
> handled, so this parameter is ignored by my server. This means that a request that
> is technically malformed will still return the requested file, but upon testing,
> many web servers that host common pages on the internet also respond despite a
> malformed HTTP version, so I intentionally left this as such.
> 
> Finally, it is worth nothing that I never implemented the SIGINT catch as the
> implementations are different per platform, and I ended up programming all the
> server functionality in before this anyway.
