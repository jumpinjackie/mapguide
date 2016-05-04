<?php
/**
 * Copyright (C) 2012-2013
 * Lars Strojny, InterNations GmbH <lars.strojny@internations.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
namespace fXmlRpc\Timing;

use Psr\Log\LoggerInterface;
use Psr\Log\LogLevel;

class Psr3TimerBridge extends AbstractTimerBridge
{
    /**
     * Create new bridge or a PSR-3 compatible logger
     *
     * Allows passing custom log level and message template (with sprintf() control characters) for log message
     * customization
     *
     * @param LoggerInterface $logger
     * @param integer         $level
     * @param string          $messageTemplate
     */
    public function __construct(LoggerInterface $logger, $level = null, $messageTemplate = null)
    {
        $this->logger = $logger;
        $this->setLevel($level, LogLevel::DEBUG);
        $this->messageTemplate = $messageTemplate ?: $this->messageTemplate;
    }

    public function recordTiming($callTime, $method, array $arguments)
    {
        $this->logger->log(
            $this->getLevel($callTime),
            sprintf($this->messageTemplate, $callTime),
            ['xmlrpcMethod' => $method, 'xmlrpcArguments' => $arguments]
        );
    }
}
