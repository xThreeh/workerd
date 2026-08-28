// Copyright (c) 2026 Cloudflare, Inc.
// Licensed under the Apache 2.0 license found in the LICENSE file or at:
//     https://opensource.org/licenses/Apache-2.0

#include "sentry.h"

#include <kj/test.h>

namespace workerd {
namespace {

void expectSentryDisposition(kj::StringPtr disposition) {
  auto exception = KJ_EXCEPTION(FAILED, "test-error");
  exception.setDetail(SENTRY_DISPOSITION_DETAIL_ID, kj::heapArray(disposition.asBytes()));

  auto loggingException = KJ_ASSERT_NONNULL(_::tagSentryExceptionForLogging(exception));
  KJ_EXPECT(loggingException.getDescription() == kj::str(disposition, " test-error"));

  KJ_EXPECT_LOG(ERROR, disposition);
  LOG_EXCEPTION("sentryDispositionTest", exception);
  KJ_EXPECT_LOG(ERROR, disposition);
  auto wdErrId = makeInternalErrorId();
  LOG_EXCEPTION_WITH_ID("sentryDispositionWithIdTest", exception, wdErrId);
  KJ_EXPECT(exception.getDescription() == "test-error", exception);
}

KJ_TEST("Sentry dispositions are applied only when logging") {
  expectSentryDisposition("SENTRY_DO"_kj);
  expectSentryDisposition("SENTRY_RT"_kj);
  expectSentryDisposition("NOSENTRY"_kj);
}

}  // namespace
}  // namespace workerd
